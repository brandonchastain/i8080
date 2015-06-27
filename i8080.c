#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEBUG 1

typedef struct conditionCodes {
	uint8_t z:1; //zero
	uint8_t s:1; //sign, 1 if -, 0 if +
	uint8_t p:1; //parity, 1 if even, 0 if odd
	uint8_t cy:1; //carry
	uint8_t ac:1; //not implemented, not used by Space Invaders
	uint8_t   :3; //padding
} conditionCodes;

typedef struct state8080 {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
	uint8_t *memory;
	struct conditionCodes cc;
	uint8_t int_enable;
} state8080;

typedef enum {ADD, SUB} carry_kind;
typedef enum {BC, DE, HL} registerPair_kind;

//emulating operations
void unimplementedInstr(state8080*);
void emulateOp(state8080*);

//operations
void addToA(state8080*, uint8_t);
void adcToA(state8080*, uint8_t);
void subFromA(state8080*, uint8_t);
void sbbFromA(state8080*, uint8_t);
void addRPtoHL(state8080*, registerPair_kind);
void decrRP(state8080*, registerPair_kind);
void daa(state8080*);

void jmp(state8080*, uint8_t*);
void call(state8080*, uint8_t*);
void ret(state8080*);

//condition flags
void setZFlag(state8080*, uint16_t);
void setSFlag(state8080*, uint16_t);
void setCYFlag(state8080*, uint8_t, uint8_t, carry_kind);
void setPFlag(state8080*, uint16_t);
void printFlags(state8080*);

//Memory utility
uint16_t getMemOffset(state8080*);

void unimplementedInstr(state8080 *state) {
	state->pc -= 1;
	printf("Error: Unimplemented instruction $%02x @ address $%04x\n", state->memory[state->pc], state->pc);
	exit(1);
}

void emulateOp(state8080 *state) {
	uint16_t answer;
	uint8_t *opcode = &(state->memory[state->pc]);
	uint16_t offset = getMemOffset(state);
	state->pc += 1;
	switch (*opcode) {
		case 0x00: break; //NOP
		case 0x01: //LXI B,word
			if(DEBUG) printf("LXI B\n");
			state->b = opcode[2];
			state->c = opcode[1];
			state->pc += 2;
			break;
        //Branching----------------------------
        //JMP addr
		case 0xc3: //JMP
			if(DEBUG) printf("JMP $%02x%02x\n", opcode[2], opcode[1]);
            jmp(state, opcode);
			break;
        //Jcondition addr (if (cond) pc <- addr, else pc <- pc + 2
        case 0xc2: //JNZ
            if(DEBUG) printf("JNZ $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.z == 0){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xca: //JZ
            if(DEBUG) printf("JZ $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.z) {
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xd2: //JNC
            if(DEBUG) printf("JNC $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.cy == 0){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xda: //JC
            if(DEBUG) printf("JC $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.cy){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        //it's possible I have the next two mixed up
        case 0xe2: //JPO
            if(DEBUG) printf("JPO $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.p == 0){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xea: //JPE
            if(DEBUG) printf("JPE $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.p){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xf2: //JP
            if(DEBUG) printf("JP $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.s == 0){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        case 0xfa: //JM
            if(DEBUG) printf("JM $%02x%02x\n", opcode[2], opcode[1]);
            if (state->cc.s){
                jmp(state, opcode);
            } else {
                state->pc += 2;
            }
            break;
        //CALL
        case 0xcd:
            call(state, opcode);
            break;


		//Arithmetic----------------------------
		// Register Form---
		// ADD reg (A <- A + reg)
		case 0x80: //ADD B
			if(DEBUG) printf("Adding B to A.\t");
			addToA(state, state->b);
			break;
		case 0x81: //C
			if(DEBUG) printf("Adding C to A.\t");
			addToA(state, state->c);
			break;
		case 0x82: //D
			if(DEBUG) printf("Adding D to a.\t");
			addToA(state, state->d);
			break;
		case 0x83: //E
			if(DEBUG) printf("Adding E to a.\t");
			addToA(state, state->e);
			break;
		case 0x84: //H
			if(DEBUG) printf("Adding H to a.\t");
			addToA(state, state->h);
			break;
		case 0x85: //L
			if(DEBUG) printf("Adding L to a.\t");
			addToA(state, state->l);
			break;
		case 0x87: //A
			if(DEBUG) printf("Adding A to A.\t");
			addToA(state, state->a);
			break;

		//ADC reg (A <- A + reg + CY)
		case 0x88: //B
			if(DEBUG) printf("Adc B\t");
			adcToA(state, state->b);
			break;
		case 0x89: //C
			if(DEBUG) printf("Adc C\t");
			adcToA(state, state->c);
			break;
		case 0x8a: //D
			if(DEBUG) printf("Adc D\t");
			adcToA(state, state->d);
			break;
		case 0x8b: //E
			if(DEBUG) printf("Adc E\t");
			adcToA(state, state->e);
			break;
		case 0x8c: //H
			if(DEBUG) printf("Adc H\t");
			adcToA(state, state->h);
			break;
		case 0x8d: //L
			if(DEBUG) printf("Adc L\t");
			adcToA(state, state->l);
			break;
		case 0x8f: //A
			if(DEBUG) printf("Adc A\t");
			adcToA(state, state->a);
			break;
		
		//SUB reg (A <- A - reg)
		case 0x90:
			if(DEBUG) printf("Sub B from A\t");
			subFromA(state, state->b);
			break;
		case 0x91:
			if(DEBUG) printf("Sub C from A\t");
			subFromA(state, state->c);
			break;
		case 0x92:
			if(DEBUG) printf("Sub D from A\t");
			subFromA(state, state->d);
			break;
		case 0x93:
			if(DEBUG) printf("Sub E from A\t");
			subFromA(state, state->e);
			break;
		case 0x94:
			if(DEBUG) printf("Sub H from A\t");
			subFromA(state, state->h);
			break;
		case 0x95:
			if(DEBUG) printf("Sub L from A\t");
			subFromA(state, state->l);
			break;
		case 0x97:
			if(DEBUG) printf("Sub A from A\t");
			subFromA(state, state->a);
			break;

		//SBB reg (A <- A - reg - CY)
		case 0x98:
			if(DEBUG) printf("Sbb B from A\t");
			sbbFromA(state, state->b);
			break;
		case 0x99:
			if(DEBUG) printf("Sbb C from A\t");
			sbbFromA(state, state->c);
			break;
		case 0x9a:
			if(DEBUG) printf("Sbb D from A\t");
			sbbFromA(state, state->d);
			break;
		case 0x9b:
			if(DEBUG) printf("Sbb E from A\t");
			sbbFromA(state, state->e);
			break;
		case 0x9c:
			if(DEBUG) printf("Sbb H from A\t");
			sbbFromA(state, state->h);
			break;
		case 0x9d:
			if(DEBUG) printf("Sbb L from A\t");
			sbbFromA(state, state->l);
			break;
		case 0x9f:
			if(DEBUG) printf("Sbb A from A\t");
			sbbFromA(state, state->a);
			break;

		//INR reg (reg <- reg + 1)
		case 0x04: //B
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->b += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x0c: //C
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->c += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x14: //D
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->d += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x1c: //E
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->e += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x24: //H
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->h += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x2c: //L
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->l += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
					//mem
		case 0x3c: //A
			if(DEBUG) printf("Inr reg $%02x\n", *opcode);
			answer = state->a += 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;

		//DCR reg (reg <- reg + 1)
		case 0x05: //B
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->b -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x0D: //C
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->c -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x15: //D
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->d -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x1d: //E
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->e -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x25: //H
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->h -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x2d: //L
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->l -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;
		case 0x3d: //A
			if(DEBUG) printf("dcr reg $%02x\n", *opcode);
			answer = state->a -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;

		//INX rp (rp <- rp + 1) rp = {BC, DE, HL}
		case 0x03://BC
			if(DEBUG) printf("inx b\t");
			uint16_t c = (uint16_t)state->c + 1;
			if (c > 0xff) {
				state->b += 1;
			}
			state->c = c & 0xff;
			if(DEBUG) printf("inx result: $%02x%02x\n", state->b, state->c);
			break;
		case 0x13://DE
			if(DEBUG) printf("inx d\t");
			uint16_t e = (uint16_t)state->e + 1;
			if (e > 0xff) {
				state->d += 1;
			}
			state->e = e & 0xff;
			if(DEBUG) printf("inx result: $%02x%02x\n", state->d, state->e);
			break;
		case 0x23://HL
			if(DEBUG) printf("inx h\t");
			uint16_t l = (uint16_t)state->l + 1;
			if (l > 0xff) {
				state->h += 1;
			}
			state->l = l & 0xff;
			if(DEBUG) printf("inx result: $%02x%02x\n", state->b, state->c);
			break;
		//DCX rp (rp <- rp - 1), rp = {BC, DE, HL}
		case 0x0b://BC
			if(DEBUG) printf("dcx b\t");
			decrRP(state, BC);
			if(DEBUG) printf("dcx result: $%02x%02x\n", state->b, state->c);
			break;
		case 0x1b://DE
			if(DEBUG) printf("dcx d\t");
			// uint16_t e = (uint16_t)state->e;
			// if (e == 0x00) {
			// 	state->d -= 1;
			// }
			// state->e = (e - 1) & 0xff;
			decrRP(state, DE);
			if(DEBUG) printf("dcx result: $%02x%02x\n", state->d, state->e);
			break;
		case 0x2b://HL
			if(DEBUG) printf("dcx h\t");
			decrRP(state, HL);
			if(DEBUG) printf("dcx result: $%02x%02x\n", state->h, state->l);
			break;
		//DAD rp (HL <- HL + rp)
		case 0x09: //BC
			if(DEBUG) printf("DAD BC\t");
			addRPtoHL(state, BC);
			if(DEBUG) printf("DAD result: %02x%02x\n", state->h, state->l);
			break;
		case 0x19: //DE
			if(DEBUG) printf("DAD DE\t");
			addRPtoHL(state, DE);
			if(DEBUG) printf("DAD result: %02x%02x\n", state->h, state->l);
			break;
		case 0x29: //HL
			if(DEBUG) printf("DAD HL\t");
			addRPtoHL(state, HL);
			if(DEBUG) printf("DAD result: %02x%02x\n", state->h, state->l);
			break;
        //DAA
        //Special instruction. See codebook for details.
        case 0x27:
            if(DEBUG) printf("DAA\t");
            daa(state);
            if(DEBUG) printf("DAA result: %02x\n", state->a);
            break;

		//Memory Form---
		//ADD M (A <- mem[HL])
		case 0x86:
			if(DEBUG) printf("Adding Mem[HL] to A.\t");
			addToA(state, state->memory[offset]);
			break;
		//ADC M (A <- mem[HL] + CY)
		case 0x8e: //Mem
			if(DEBUG) printf("Adc Mem[HL]\t");
			adcToA(state, state->memory[offset]);
			break;
		//SUB M (A <- mem[HL])
		case 0x96:
			if(DEBUG) printf("Sub Mem[HL]\t");
			subFromA(state, state->memory[offset]);
			break;
		//SBB M (A <- mem[HL] - CY)
		case 0x9e:
			if(DEBUG) printf("Sbb Mem[HL]\t");
			sbbFromA(state, state->memory[offset]);
			break;
		//INR M (mem[HL] <- mem[HL] + 1)
		case 0x34:
			if(DEBUG) printf("INR Mem[HL]\n");
			uint8_t ans = state->memory[offset] += 1;
			setZFlag(state, ans);
			setSFlag(state, ans);
			setPFlag(state, ans);
			break;
		//DCR M (mem[HL] <- mem[HL] - 1)
		case 0x35:
			if(DEBUG) printf("DCR Mem[HL]\n");
			answer = state->memory[offset] -= 1;
			setZFlag(state, answer);
			setSFlag(state, answer);
			setPFlag(state, answer);
			break;

		//Immediate Form---
		//ADI byte (A <- A + D8)
		case 0xc6: //ADI byte, imm add
			if(DEBUG) printf("Adding $%02x to A.\t", opcode[1]);
			addToA(state, opcode[1]);
			state->pc += 1;
			break;
		//ACI byte (A <- A + D8 + CY)
		case 0xce:
			if(DEBUG) printf("Adc $%02x to A\t", opcode[1]);
			adcToA(state, opcode[1]);
			state->pc += 1;
			break;
		//SUI byte (A <- A - D8)
		case 0xd6:
			if(DEBUG) printf("SUI $%02x from A\t", opcode[1]);
			subFromA(state, opcode[1]);
			state->pc += 1;
			break;
		//SBI byte (A <- A - D8 - cy)
		case 0xde:
			if(DEBUG) printf("SBI $%02x from A\t", opcode[1]);
			sbbFromA(state, opcode[1]);
			state->pc += 1;
			break;

		default: unimplementedInstr(state); break;
	}
}

uint16_t getMemOffset(state8080 *state) {
	return (state->h<<8) | (state->l);
}

void addToA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a + (uint16_t)value;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, ADD);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Add result: $%02x\t", state->a);
	printFlags(state);
}

void adcToA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a + (uint16_t)value;
	answer += state->cc.cy;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, ADD);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Adc result: $%02x\t", state->a);
	printFlags(state);
}

void subFromA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a - (uint16_t)value;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setPFlag(state, answer);
	setCYFlag(state, state->a, value, SUB);
	state->a = answer & 0xff;
	if(DEBUG) printf("SUB result: $%02x\t", state->a);
	printFlags(state);
}

void sbbFromA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a - (uint16_t)value;
	answer -= (uint16_t)state->cc.cy;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, SUB);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Sbb result: $%02x\t", state->a);
	printFlags(state);
}

void decrRP(state8080 *state, registerPair_kind rp){
	uint8_t *rh, *rl;
	switch(rp) {
		case BC: rh = &state->b; rl = &state->c; break;
		case DE: rh = &state->d; rl = &state->e; break;
		case HL: rh = &state->h; rl = &state->l; break;
	}

	uint16_t temp = (uint16_t)*rl;
	if (temp == 0x00) {
		*rh -= 1;
	}
	*rl = (*rl - 1) & 0xff;
}

void addRPtoHL(state8080 *state, registerPair_kind rp) {
	uint32_t rp1;
	uint32_t rp2 = (state->h << 8) | state->l;
	switch(rp) {
		case BC:
			rp1 = (state->b << 8) | state->c;
			break;
		case DE:
			rp1 = (state->d << 8) | state->e;
			break;
		case HL:
		default:
			rp1 = (state->h << 8) | state->l;
			break;
	}
	uint32_t answer = rp1 + rp2;
	state->cc.cy = answer > 0xffff;
	state->h = (answer >> 8) & 0xff;
	state->l = answer & 0xff;
}

void daa(state8080 *state) {
    if ((state->a & 0x0f) > 9 || state->cc.ac) {
        state->a += 6;
    }
    if ((state->a >> 4) > 9 || state->cc.cy) {
        uint16_t res = (uint16_t) state->a + 0x60;
        setZFlag(state, res);
        setSFlag(state, res);
        setCYFlag(state, state->a, 0x60, ADD);
        setPFlag(state, res);
        state->a = res & 0xff;
    }
}

void jmp(state8080 *state, uint8_t *opcode) {
    state->pc = (opcode[2] << 8) | (opcode[1]);
}

void call(state8080 *state, uint8_t *opcode) {
    uint16_t ret = state->pc + 2;
    state->memory[state->sp-1] = (ret >> 8) & 0xff;
    state->memory[state->sp-2] = (ret >> 8) & 0xff;
    state->sp -= 2;
    jmp(state, opcode);
}

void ret(state8080 *state) {
    state->pc = (state->memory[(state->sp + 1)] << 8)
                | state->memory[state->sp];
    state->sp += 2;
}

void setZFlag(state8080 *state, uint16_t answer) {
	state->cc.z = !(answer & 0xff);
}

void setSFlag(state8080 *state, uint16_t answer) {
	state->cc.s = answer & 0x80;
}

void setCYFlag(state8080 *state, uint8_t a, uint8_t b, carry_kind kind) {
	//TODO: This might be incorrect.
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
	if(DEBUG) printf("Z:%1d S:%1d P:%1d CY:%1d\n", state->cc.z, state->cc.s, state->cc.p, state->cc.cy);
}

int main(int argc, char **argv) {
	state8080 state = {0};

	FILE *f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("Error: Couldn't open %s\n", argv[1]);
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);
	uint8_t *buffer = (uint8_t *)malloc(fsize);
	fread(buffer, fsize, 1, f);
	fclose(f);

	state.memory = buffer;

	while (state.pc < fsize) {
		emulateOp(&state);
	}

	free(buffer);
	return 0;
}
