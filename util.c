#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "globals.h"

void invalidInstr(state8080*);

void setZFlag(state8080 *state, uint16_t answer) {
	state->cc.z = !(answer & 0xff);
}

void setSFlag(state8080 *state, uint16_t answer) {
	state->cc.s = answer & 0x80;
}

void setCYFlag(state8080 *state, uint8_t a, uint8_t b, carry_kind kind) {
	//This looks useful: http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	if (kind == ADD) {
		uint16_t answer = (uint16_t)a + (uint16_t)b;
		state->cc.cy = answer > 0xff;
	} else {
		state->cc.cy = a < b;
	}
}

void setPFlag(state8080 *state, uint16_t answer) {
	state->cc.p = ~(answer & 0x01);
}

void printFlags(state8080 *state) {
	printf("\tZ:%1d S:%1d P:%1d CY:%1d\n", state->cc.z, state->cc.s, state->cc.p, state->cc.cy);
}

void printMem(state8080 *state) {
	printf("(HL): #$%02x\n", state->memory[(state->h << 8) | state->l]);
	printf("(SP+1): #$%02x\n", state->memory[state->sp + 1]);
    printf("(SP): #$%02x\n", state->memory[state->sp]);
}

void debugPrint(state8080 *state) {
    printf("Regs:\n");
    printf("\tA: $%02x\tSP:$%04x\n", state->a, state->sp);
    printf("\tB: $%02x\tC: $%02x\n", state->b, state->c);
    printf("\tD: $%02x\tE: $%02x\n", state->d, state->e);
    printf("\tH: $%02x\tL: $%02x\n", state->h, state->l);
    printf("Flags:\n");
    printFlags(state);
	printf("Mem:\n");
	printMem(state);
	printf("\n");
}

uint16_t getMemOffset(state8080 *state) {
	return (state->h<<8) | (state->l);
}

register_kind getRegFromNumber(uint8_t regno) {
    register_kind reg = regs[regno];
    return reg;
}

registerPair_kind getRPFromNumber(state8080 *state, uint8_t regPairNo) {
	if (regPairNo < 0 || regPairNo > 4) {
		invalidInstr(state);
	}
	registerPair_kind rp = rps[regPairNo];
	return rp;
}


uint8_t getRegVal(state8080 *state, register_kind reg) {
    switch (reg) {
        case A:
            return state->a;
        case B:
            return state->b;
        case C:
            return state->c;
        case D:
            return state->d;
        case E:
            return state->e;
        case H:
            return state->h;
        case L:
            return state->l;
        case M:
            return state->memory[getMemOffset(state)];
    }
}

uint16_t getRPVal(state8080 *state, registerPair_kind rp) {
	uint8_t hi;
	uint8_t lo;
	switch(rp) {
		case BC: hi = state->b; lo = state->c; break;
		case DE: hi = state->d; lo = state->e; break;
		case HL: hi = state->h; lo = state->l; break;
		case SP: hi = (state->sp & 0xff00) >> 8; lo = state->sp & 0x00ff; break;
	}
	return (uint16_t)((hi << 8) | lo);
}

void setRegVal(state8080 *state, register_kind reg, uint8_t val) {
    switch (reg) {
        case A:
            state->a = val;
            break;
        case B:
            state->b = val;
            break;
        case C:
            state->c = val;
            break;
        case D:
            state->d = val;
            break;
        case E:
            state->e = val;
            break;
        case H:
            state->h = val;
            break;
        case L:
            state->l = val;
            break;
        case M:
            state->memory[getMemOffset(state)] = val;
            break;
        default:
            invalidInstr(state);
            break;
    }
}

void setRPVal(state8080 *state, registerPair_kind rp, uint16_t val) {
	if (rp == SP) {
		state->sp = val;
	} else {
		uint8_t hi = (uint8_t)(val >> 8);
		uint8_t lo = (uint8_t)(val & 0x00ff);
		switch (rp) {
			case BC:
				state->b = hi;
				state->c = lo;
				break;
			case DE:
				state->d = hi;
				state->e = lo;
				break;
			case HL:
				state->h = hi;
				state->l = lo;
				break;
			case SP: break;
		}
	}
}

char* getRegLabel(state8080 *state, register_kind reg) {
    switch (reg) {
        case A:
            return "a";
        case B:
            return "b";
        case C:
            return "c";
        case D:
            return "d";
        case E:
            return "e";
        case H:
            return "h";
        case L:
            return "l";
        case M:
            return "mem";
        default:
            invalidInstr(state);
			return "-1";
    }
}

char* getRPLabel(state8080 *state, registerPair_kind rp) {
    switch (rp) {
        case BC: return "BC";
        case DE: return "DE";
        case HL: return "HL";
        case SP: return "SP";
        default:
            invalidInstr(state);
            return "-1";
    }
}

void unimplementedInstr(state8080 *state) {
	state->pc -= 1;
	printf("Error: Unimplemented instruction $%02x @ address $%04x\n", 
            state->memory[state->pc], state->pc);
    free(state->memory);
    exit(1);
}

void invalidInstr(state8080 *state) {
    state->pc -= 1;
    printf("Error: Invalid instruction $%02x @ address $%04x\n",
            state->memory[state->pc], state->pc);
    free(state->memory);
    exit(1);
}