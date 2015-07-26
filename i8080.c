#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "disassembler.h"
#include "globals.h"
#include "util.h"

#include "arithmetic.h"
#include "branching.h"
#include "logical.h"
#include "dataTransfer.h"
#include "stack.h"

register_kind regs[8] = {B, C, D, E, H, L, M, A};
registerPair_kind rps[4] = {BC, DE, HL, SP};
uint8_t isStepMode = 0;

void emulateOp(state8080*);

void emulateOp(state8080 *state) {
	uint16_t answer;
	uint8_t *opcode = &(state->memory[state->pc]);
	uint16_t offset = getMemOffset(state);
	state->pc += 1;
	switch (*opcode) {
		case 0x00: break; //NOP
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
            if (DEBUG) printf("CALL $%02x%02x\n", opcode[2], opcode[1]);
            call(state, opcode);
            break;
        //RET
        case 0xc9:
            if(DEBUG) printf("RET\n");
            ret(state);
            break;
        //Rcond
        case 0xc0:
            if (DEBUG) printf("RNZ\n");
            if (!state->cc.z) ret(state);
            break;
        case 0xc8:
            if (DEBUG) printf("RZ\n");
            if (state->cc.z) ret(state);
            break;
        case 0xd0:
            if (DEBUG) printf("RNC\n");
            if (!state->cc.cy) ret(state);
            break;
        case 0xd8:
            if (DEBUG) printf("RC\n");
            if (state->cc.cy) ret(state);
            break;
        case 0xe0:
            if (DEBUG) printf("RPO\n");
            if (!state->cc.p) ret(state);
            break;
        case 0xe8:
            if (DEBUG) printf("RPE\n");
            if (state->cc.p) ret(state);
            break;
        case 0xf0:
            if (DEBUG) printf("RP\n");
            if (!state->cc.s) ret(state);
            break;
        case 0xf8:
            if (DEBUG) printf("RM\n");
            if (state->cc.s) ret(state);
            break;
        //RST n
        case 0xc7:
            if (DEBUG) printf("RST 0\n");
            rst(state, 0);
            break;
        case 0xcf:
            if (DEBUG) printf("RST 1\n");
            rst(state, 1);
            break;
        case 0xd7:
            if (DEBUG) printf("RST 2\n");
            rst(state, 2);
            break;
        case 0xdf:
            if (DEBUG) printf("RST 3\n");
            rst(state, 3);
            break;
        case 0xe7:
            if (DEBUG) printf("RST 4\n");
            rst(state, 4);
            break;
        case 0xef:
            if (DEBUG) printf("RST 5\n");
            rst(state, 5);
            break;
        case 0xf7:
            if (DEBUG) printf("RST 6\n");
            rst(state, 6);
            break;
        case 0xff:
            if (DEBUG) printf("RST 7\n");
            rst(state, 7);
            break;
        //PCHL
        case 0xe9:
            if (DEBUG) printf("PCHL\n");
            pchl(state);
            break;

        //Logical-------------------------------
        //ANA r
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xa7:
            ana(state, *opcode);
            break;
        case 0xe6:
            ani(state, opcode);
            state->pc += 1;
            break;
        case 0xa8:
        case 0xa9:
        case 0xaa:
        case 0xab:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xaf:
            xra(state, *opcode);
            break;
        case 0xee:
            xri(state, opcode);
            state->pc += 1;
            break;
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
            ora(state, *opcode);
            break;
        case 0xf6:
            ori(state, opcode);
            state->pc += 1;
            break;
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:
            cmp(state, *opcode);
            break;
        case 0xfe:
            cpi(state, opcode);
            state->pc += 1;
            break;
        case 0x07:
            rlc(state, *opcode);
            break;
        case 0x0f:
            rrc(state, *opcode);
            break;
        case 0x17:
            ral(state, *opcode);
            break;
        case 0x18:
            invalidInstr(state);
            break;
        case 0x1f:
            rar(state, *opcode);
            break;
        case 0x2f:
            cma(state);
            break;
        case 0x3f:
            cmc(state);
            break;
        case 0x37:
            stc(state);
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

        //Data Transfer Group
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
        case 0x44:
        case 0x45:
        case 0x46:
        case 0x47:
        case 0x48:
        case 0x49:
        case 0x4a:
        case 0x4b:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x4f:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0x56:
        case 0x57:
        case 0x58:
        case 0x59:
        case 0x5a:
        case 0x5b:
        case 0x5c:
        case 0x5d:
        case 0x5e:
        case 0x5f:
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6a:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7a:
        case 0x7b:
        case 0x7c:
        case 0x7d:
        case 0x7e:
        case 0x7f:
            mov(state, *opcode);
            break;
		case 0x06:
		case 0x0e:
		case 0x16:
		case 0x1e:
		case 0x26:
		case 0x2e:
		case 0x36:
		case 0x3e:
			mvi(state, opcode);
			state->pc += 1;
			break;
		case 0x01:
		case 0x11:
		case 0x21:
		case 0x31:
			lxi(state, opcode);
			state->pc += 2;
			break;
		case 0x3a:
			lda(state, opcode);
			state->pc += 2;
			break;
		case 0x32:
			sta(state, opcode);
			state->pc += 2;
			break;
		case 0x2a:
			lhld(state, opcode);
			state->pc += 2;
			break;
		case 0x22:
			shld(state, opcode);
			state->pc += 2;
			break;
		case 0x0a:
		case 0x1a:
			ldax(state, opcode);
			break;
		case 0x02:
		case 0x12:
			stax(state, opcode);
			break;
		case 0xeb:
			xchg(state, opcode);
			break;
        case 0xc5:
        case 0xd5:
        case 0xe5:
        //case 0xf5: //PSW
            push(state, *opcode);
            break;
        case 0xf5:
        	pushPsw(state);
        	break;
        case 0xc1:
        case 0xd1:
        case 0xe1:
            pop(state, *opcode);
            break;
         case 0xf1:
         	popPsw(state);
         	break;
        case 0xf9:
            sphl(state);
            break;
        //case 0x76:
            //TODO: halt instruction.
            break;

		default: unimplementedInstr(state); break;
	}
}









int main(int argc, char **argv) {
	state8080 state1 = {0};
    state8080 *state = &state1;

	if (argc > 2) {
		if (strcmp(argv[1], "-d") == 0){
			isStepMode = 1;
		}
	}

	FILE *f = fopen(argv[argc - 1], "rb");
	if (f == NULL) {
		printf("Error: Couldn't open %s\n", argv[1]);
		exit(1);
	}

	fseek(f, 0L, SEEK_END);
	int fsize = ftell(f);
	//INFO: instr size limited to 1024 bytes right now.
	state->memSize = fsize + 1024;
	fseek(f, 0L, SEEK_SET);
	uint8_t *buffer = (uint8_t *)calloc(1, state->memSize);
	fread(buffer, fsize, 1, f);
	fclose(f);

	state->memory = buffer;

    while (state->pc < fsize) {
		disassemble((char *)state->memory, state->pc);
    	emulateOp(state);
        if (DEBUG) printFlags(state);
        if (isStepMode) {
			debugPrint(state);
			printf("Press [Enter] to continue.\n");
            //wait for user to hit enter
			fflush(stdin);
			getchar();
        }
    }

	free(buffer);
	return 0;
}
