#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct conditionCodes {
	uint8_t z:1;
	uint8_t s:1;
	uint8_t p:1;
	uint8_t cy:1;
	uint8_t ac:1;
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
			state->b = opcode[2];
			state->c = opcode[1];
			state->pc += 2;
			break;
		default: unimplementedInstr(state); break;
	}
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