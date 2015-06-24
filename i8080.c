#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEBUG 1

typedef struct conditionCodes {
	uint8_t z:1;
	uint8_t s:1;
	uint8_t p:1;
	uint8_t cy:1;
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

void unimplementedInstr(state8080*);
void emulateOp(state8080*);
void addToA(state8080*, uint8_t);
void setZFlag(state8080*, uint16_t);
void setSFlag(state8080*, uint16_t);
void setCYFlag(state8080*, uint16_t);
void setPFlag(state8080*, uint16_t);

void unimplementedInstr(state8080 *state) {
	state->pc -= 1;
	printf("Error: Unimplemented instruction $%02x @ address $%04x\n", state->memory[state->pc], state->pc);
	exit(1);
}

void emulateOp(state8080 *state) {
	uint8_t *opcode = &(state->memory[state->pc]);
	state->pc += 1;
	switch (*opcode) {
		case 0x00: break; //NOP
		case 0x01: //LXI B,word
			if(DEBUG) printf("LXI B\n");
			state->b = opcode[2];
			state->c = opcode[1];
			state->pc += 2;
			break;

		//Arithmetic----------------------------

		// Register Form---
		// ADD reg (A <- A + reg)
		case 0x80: //ADD B
			if(DEBUG) printf("Adding B to A.\n");
			addToA(state, state->b);
			break;
		case 0x81: //C
			if(DEBUG) printf("Adding C to A.\n");
			addToA(state, state->c);
			break;
		case 0x82: //D
			if(DEBUG) printf("Adding D to a.\n");
			addToA(state, state->d);
			break;
		case 0x83: //E
			if(DEBUG) printf("Adding E to a.\n");
			addToA(state, state->e);
			break;
		case 0x84: //H
			if(DEBUG) printf("Adding H to a.\n");
			addToA(state, state->h);
			break;
		case 0x85: //L
			if(DEBUG) printf("Adding L to a.\n");
			addToA(state, state->l);
			break;
		case 0x87: //A
			if(DEBUG) printf("Adding A to A.\n");
			addToA(state, state->a);
			break;

		//Memory Form---
		//ADD M (A <- mem[HL])
		case 0x86:
			if(DEBUG) printf("Adding Mem[HL] to A.\n");
			uint16_t offset = (state->h<<8) | (state->l);
			addToA(state, state->memory[offset]);
			break;
		
		//Immediate Form---
		//ADI byte (A <- A + D8)
		case 0xc6: //ADI byte, imm add
			if(DEBUG) printf("Adding imm %02x to A.\n", opcode[1]);
			addToA(state, opcode[1]);
			break;

		default: unimplementedInstr(state); break;
	}
}

void addToA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a + (uint16_t)value;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, answer);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Add result: $%02x\n", state->a);
}

void setZFlag(state8080 *state, uint16_t answer) {
	state->cc.z = !(answer & 0xff);
}

void setSFlag(state8080 *state, uint16_t answer) {
	state->cc.s = answer & 0x80;
}

void setCYFlag(state8080 *state, uint16_t answer) {
	state->cc.cy = answer > 0xff;
}

void setPFlag(state8080 *state, uint16_t answer) {
	state->cc.p = ~(answer & 0x01);
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