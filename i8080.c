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

typedef enum {ADD, SUB} carry_kind;

void unimplementedInstr(state8080*);
void emulateOp(state8080*);

void addToA(state8080*, uint8_t);
void adcToA(state8080*, uint8_t);
void subFromA(state8080 *, uint8_t);

void setZFlag(state8080*, uint16_t);
void setSFlag(state8080*, uint16_t);
void setCYFlag(state8080*, uint16_t, carry_kind);
void setPFlag(state8080*, uint16_t);
void printFlags(state8080*);

uint16_t getMemOffset(state8080*);


void unimplementedInstr(state8080 *state) {
	state->pc -= 1;
	printf("Error: Unimplemented instruction $%02x @ address $%04x\n", state->memory[state->pc], state->pc);
	exit(1);
}

void emulateOp(state8080 *state) {
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
		case 0xc3: //JMP
			if(DEBUG) printf("JMP $%02x%02x\n", opcode[2], opcode[1]);
			state->pc = (opcode[2] << 8) | (opcode[1]);
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
		case 0x99:
		case 0x9a:
		case 0x9b:
		case 0x9c:
		case 0x9d:
		case 0x9f:
			//TODO
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
			//TODO
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
	setCYFlag(state, answer, ADD);
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
	setCYFlag(state, answer, ADD);
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
	setCYFlag(state, answer, SUB);
	if(DEBUG) printf("SUB result: $%02x\t", state->a);
	printFlags(state);
}

void sbbFromA(state8080 *state, uint8_t value){
	//TODO
}

void setZFlag(state8080 *state, uint16_t answer) {
	state->cc.z = !(answer & 0xff);
}

void setSFlag(state8080 *state, uint16_t answer) {
	state->cc.s = answer & 0x80;
}

void setCYFlag(state8080 *state, uint16_t answer, carry_kind kind) {
	//TODO: This might be incorrect.
	//This looks useful: http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	if (kind == ADD) {
		state->cc.cy = answer > 0xff;
	} else {
		state->cc.cy = answer < 0;
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