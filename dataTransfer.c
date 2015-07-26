#include <stdint.h>
#include <stdio.h>
#include "globals.h"
#include "util.h"

void mov(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("MOV\t");
    int dregno = (opcode >> 3) & 0x07;
    int sregno = opcode & 0x07;
	printf("dreg: %d", dregno);
	printf("sreg: %d\n", sregno);
    register_kind dreg = getRegFromNumber(dregno);
    register_kind sreg = getRegFromNumber(sregno);
    if (DEBUG) printf("%s to %s\n", getRegLabel(state, sreg), getRegLabel(state, dreg));
    uint8_t val = getRegVal(state, sreg);
    setRegVal(state, dreg, val);
}

void mvi(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("MVI\t");
	uint8_t dregno = (*opcode >> 3) & 0x07;
	register_kind dreg = getRegFromNumber(dregno);
	uint8_t val = opcode[1];
	if (DEBUG) printf("#$%02x to %s\n", val, getRegLabel(state, dreg));
	setRegVal(state, dreg, val);
}

void lxi(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("LXI\t");
	uint8_t regPairNo = (*opcode >> 4) & 0x03;
	uint16_t val = (opcode[2] << 8) | opcode[1];
	registerPair_kind rp = getRPFromNumber(state, regPairNo);
	setRPVal(state, rp, val);
	if (DEBUG) printf("result: #$%04x\n", getRPVal(state, rp));
}

void lda(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("LDA\t");
	uint16_t addr = (opcode[2] << 8) | opcode[1];
	state->a = state->memory[addr];
	if (DEBUG) printf("result: #$%02x\n", state->a);
}

void sta(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("STA\t");
	uint16_t addr = (opcode[2] << 8) | opcode[1];
	if (DEBUG) printf("to ($%04x)\n", addr);
	state->memory[addr] = state->a;
}

void lhld(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("LHLD\t");
	uint16_t addr = (opcode[2] << 8) | opcode[1];
	if (DEBUG) printf("($%04x)\n", addr);
	state->h = state->memory[addr + 1];
	state->l = state->memory[addr];
}

void shld(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("SHLD\t");
	uint16_t addr = (opcode[2] << 8) | opcode[1];
	if (DEBUG) printf("($%04x)\n", addr);
	state->memory[addr + 1] = state->h;
	state->memory[addr] = state->l;
}

void ldax(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("LDAX\t");
	uint8_t regno = (*opcode) >> 4 & 0x03;
	registerPair_kind rp = getRPFromNumber(state, regno);
	uint16_t addr;
	switch (rp) {
		case BC:
			addr = (state->b << 8) | state->c;
			break;
		case DE:
			addr = (state->d << 8) | state->e;
			break;
		default:
			invalidInstr(state);
			break;
	}

	state->a = state->memory[addr];
	if (DEBUG) printf("result: #$%02x\n", state->a);
}

void stax(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("STAX\t");
	uint8_t regno = (*opcode) >> 4 & 0x03;
	registerPair_kind rp = getRPFromNumber(state, regno);
	uint16_t addr;
	switch (rp) {
		case BC:
			addr = (state->b << 8) | state->c;
			break;
		case DE:
			addr = (state->d << 8) | state->e;
			break;
		default:
			invalidInstr(state);
			break;
	}

	state->memory[addr] = state->a;
	if (DEBUG) printf("result: #$%02x\n", state->memory[addr]);
}

void xchg(state8080* state, uint8_t *opcode) {
	if (DEBUG) printf("XCHG\n");
	uint8_t temp = state->h;
	state->h = state->d;
	state->d = temp;

	temp = state->l;
	state->l = state->e;
	state->e = temp;
}