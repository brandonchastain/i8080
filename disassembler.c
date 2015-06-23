#include <stdlib.h>
#include <stdio.h>

int disassemble(unsigned char *buffer, int pc)
{
    unsigned char *code = buffer + pc;
    int opbytes = 1;
    printf("%04x\t%02x\t", pc, *code);
    switch (*code) {
        case 0x00: printf("NOP\n"); break;
        case 0x01:
            printf("LXI\t\tB,#$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x02: printf("STAX\tB\n"); break;
        case 0x03: printf("INX\t\tB\n"); break;
        case 0x04: printf("INR\t\tB\n"); break;
        case 0x05: printf("DCR\t\tB\n"); break;
        case 0x06:
            printf("MVI\t\tB,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x07: printf("RLC\n"); break;
        case 0x09: printf("DAD\t\tB\n"); break;
        case 0x0a: printf("LDAX\tB\n"); break;
        case 0x0b: printf("DCX\t\tB\n"); break;
        case 0x0c: printf("INR\t\tC\n"); break;
        case 0x0d: printf("DCR\t\tC\n"); break;
        case 0x0e:
            printf("MVI\t\tC,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x0f: printf("RRC\n"); break;
        case 0x11:
            printf("LXI\t\tD,#$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x12: printf("STAX\tD\n"); break;
        case 0x13: printf("INX\t\tD\n"); break;
        case 0x14: printf("INR\t\tD\n"); break;
        case 0x15: printf("DCR\t\t D\n"); break;
        case 0x16:
            printf("MVI\t\tD,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x17: printf("RAL\n"); break;
        case 0x19: printf("DAD\t\tD\n"); break;
        case 0x1a: printf("LDAX\tD\n"); break;
        case 0x1b: printf("DCX\t\tD\n"); break;
        case 0x1c: printf("INR\t\tE\n"); break;
        case 0x1d: printf("DCR\t\tE\n"); break;
        case 0x1e:
            printf("MVI\t\tE,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x1f: printf("RAR\n"); break;
        case 0x20: printf("RIM\n"); break;
        case 0x21:
            printf("LXI\t\tH,#$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x22:
            printf("SHLD\t($%02x%02x)\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x23: printf("INX\t\tH\n"); break;
        case 0x24: printf("INR\t\tH\n"); break;
        case 0x25: printf("DCR\t\tH\n"); break;
        case 0x26:
            printf("MVI\t\tH,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x27: printf("DAA\n"); break;
        case 0x29: printf("DAD\t\tH\n"); break;
        case 0x2a:
            printf("LHLD\t($%02x%02x)\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x2b: printf("DCX\t\tH\n"); break;
        case 0x2c: printf("INR\t\tL\n"); break;
        case 0x2d: printf("DCR\t\tL\n"); break;
        case 0x2e:
            printf("MVI\t\tL,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x2f: printf("CMA\n"); break;
        case 0x30: printf("SIM\n"); break;
        case 0x31:
            printf("LXI\t\tSP,#$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x32:
            printf("STA\t\t($%02x%02x)\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x33: printf("INX\t\tSP\n"); break;
        case 0x34: printf("INR\t\tM\n"); break;
        case 0x35: printf("DCR\t\tM\n"); break;
        case 0x36:
            printf("MVI\t\tM,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x37: printf("STC\n"); break;
        case 0x39: printf("DAD\t\tSP\n"); break;
        case 0x3a:
            printf("LDA\t\t($%02x%02x)\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x3b: printf("DCX\t\tSP\n"); break;
        case 0x3c: printf("INR\t\tA\n"); break;
        case 0x3d: printf("DCR\t\tA\n"); break;
        case 0x3e:
            printf("MVI\t\tA,#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0x3f: printf("CMC\n"); break;
        case 0x40: printf("MOV\t\tB,B\n"); break;
        case 0x41: printf("MOV\t\tB,C\n"); break;
        case 0x42: printf("MOV\t\tB,D\n"); break;
        case 0x43: printf("MOV\t\tB,E\n"); break;
        case 0x44: printf("MOV\t\tB,H\n"); break;
        case 0x45: printf("MOV\t\tB,L\n"); break;
        case 0x46: printf("MOV\t\tB,M\n"); break;
        case 0x47: printf("MOV\t\tB,A\n"); break;
        case 0x48: printf("MOV\t\tC,B\n"); break;
        case 0x49: printf("MOV\t\tC,C\n"); break;
        case 0x4a: printf("MOV\t\tC,D\n"); break;
        case 0x4b: printf("MOV\t\tC,E\n"); break;
        case 0x4c: printf("MOV\t\tC,H\n"); break;
        case 0x4d: printf("MOV\t\tC,L\n"); break;
        case 0x4e: printf("MOV\t\tC,M\n"); break;
        case 0x4f: printf("MOV\t\tC,A\n"); break;
        case 0x50: printf("MOV\t\tD,B\n"); break;
        case 0x51: printf("MOV\t\tD,C\n"); break;
        case 0x52: printf("MOV\t\tD,D\n"); break;
        case 0x53: printf("MOV\t\tD,E\n"); break;
        case 0x54: printf("MOV\t\tD,H\n"); break;
        case 0x55: printf("MOV\t\tD,L\n"); break;
        case 0x56: printf("MOV\t\tD,M\n"); break;
        case 0x57: printf("MOV\t\tD,A\n"); break;
        case 0x58: printf("MOV\t\tE,B\n"); break;
        case 0x59: printf("MOV\t\tE,C\n"); break;
        case 0x5a: printf("MOV\t\tE,D\n"); break;
        case 0x5b: printf("MOV\t\tE,E\n"); break;
        case 0x5c: printf("MOV\t\tE,H\n"); break;
        case 0x5d: printf("MOV\t\tE,L\n"); break;
        case 0x5e: printf("MOV\t\tE,M\n"); break;
        case 0x5f: printf("MOV\t\tE,A\n"); break;
        case 0x60: printf("MOV\t\tH,B\n"); break;
        case 0x61: printf("MOV\t\tH,C\n"); break;
        case 0x62: printf("MOV\t\tH,D\n"); break;
        case 0x63: printf("MOV\t\tH,E\n"); break;
        case 0x64: printf("MOV\t\tH,H\n"); break;
        case 0x65: printf("MOV\t\tH,L\n"); break;
        case 0x66: printf("MOV\t\tH,M\n"); break;
        case 0x67: printf("MOV\t\tH,A\n"); break;
        case 0x68: printf("MOV\t\tL,B\n"); break;
        case 0x69: printf("MOV\t\tL,C\n"); break;
        case 0x6a: printf("MOV\t\tL,D\n"); break;
        case 0x6b: printf("MOV\t\tL,E\n"); break;
        case 0x6c: printf("MOV\t\tL,H\n"); break;
        case 0x6d: printf("MOV\t\tL,L\n"); break;
        case 0x6e: printf("MOV\t\tL,M\n"); break;
        case 0x6f: printf("MOV\t\tL,A\n"); break;
        case 0x70: printf("MOV\t\tM,B\n"); break;
        case 0x71: printf("MOV\t\tM,C\n"); break;
        case 0x72: printf("MOV\t\tM,D\n"); break;
        case 0x73: printf("MOV\t\tM,E\n"); break;
        case 0x74: printf("MOV\t\tM,H\n"); break;
        case 0x75: printf("MOV\t\tM,L\n"); break;
        case 0x76: printf("HLT\n"); break;
        case 0x77: printf("MOV\t\tM,A\n"); break;
        case 0x78: printf("MOV\t\tA,B\n"); break;
        case 0x79: printf("MOV\t\tA,C\n"); break;
        case 0x7a: printf("MOV\t\tA,D\n"); break;
        case 0x7b: printf("MOV\t\tA,E\n"); break;
        case 0x7c: printf("MOV\t\tA,H\n"); break;
        case 0x7d: printf("MOV\t\tA,L\n"); break;
        case 0x7e: printf("MOV\t\tA,M\n"); break;
        case 0x7f: printf("MOV\t\tA,A\n"); break;
        case 0x80: printf("ADD\t\tB\n"); break;
        case 0x81: printf("ADD\t\tC\n"); break;
        case 0x82: printf("ADD\t\tD\n"); break;
        case 0x83: printf("ADD\t\tE\n"); break;
        case 0x84: printf("ADD\t\tH\n"); break;
        case 0x85: printf("ADD\t\tL\n"); break;
        case 0x86: printf("ADD\t\tM\n"); break;
        case 0x87: printf("ADD\t\tA\n"); break;
        case 0x88: printf("ADC\t\tB\n"); break;
        case 0x89: printf("ADC\t\tC\n"); break;
        case 0x8a: printf("ADC\t\tD\n"); break;
        case 0x8b: printf("ADC\t\tE\n"); break;
        case 0x8c: printf("ADC\t\tH\n"); break;
        case 0x8d: printf("ADC\t\tL\n"); break;
        case 0x8e: printf("ADC\t\tM\n"); break;
        case 0x8f: printf("ADC\t\tA\n"); break;
        case 0x90: printf("SUB\t\tB\n"); break;
        case 0x91: printf("SUB\t\tC\n"); break;
        case 0x92: printf("SUB\t\tD\n"); break;
        case 0x93: printf("SUB\t\tE\n"); break;
        case 0x94: printf("SUB\t\tH\n"); break;
        case 0x95: printf("SUB\t\tL\n"); break;
        case 0x96: printf("SUB\t\tM\n"); break;
        case 0x97: printf("SUB\t\tA\n"); break;
        case 0x98: printf("SBB\t\tB\n"); break;
        case 0x99: printf("SBB\t\tC\n"); break;
        case 0x9a: printf("SBB\t\tD\n"); break;
        case 0x9b: printf("SBB\t\tE\n"); break;
        case 0x9c: printf("SBB\t\tH\n"); break;
        case 0x9d: printf("SBB\t\tL\n"); break;
        case 0x9e: printf("SBB\t\tM\n"); break;
        case 0x9f: printf("SBB\t\tA\n"); break;
        case 0xa0: printf("ANA\t\tB\n"); break;
        case 0xa1: printf("ANA\t\tC\n"); break;
        case 0xa2: printf("ANA\t\tD\n"); break;
        case 0xa3: printf("ANA\t\tE\n"); break;
        case 0xa4: printf("ANA\t\tH\n"); break;
        case 0xa5: printf("ANA\t\tL\n"); break;
        case 0xa6: printf("ANA\t\tM\n"); break;
        case 0xa7: printf("ANA\t\tA\n"); break;
        case 0xa8: printf("XRA\t\tB\n"); break;
        case 0xa9: printf("XRA\t\tC\n"); break;
        case 0xaa: printf("XRA\t\tD\n"); break;
        case 0xab: printf("XRA\t\tE\n"); break;
        case 0xac: printf("XRA\t\tH\n"); break;
        case 0xad: printf("XRA\t\tL\n"); break;
        case 0xae: printf("XRA\t\tM\n"); break;
        case 0xaf: printf("XRA\t\tA\n"); break;
        case 0xb0: printf("ORA\t\tB\n"); break;
        case 0xb1: printf("ORA\t\tC\n"); break;
        case 0xb2: printf("ORA\t\tD\n"); break;
        case 0xb3: printf("ORA\t\tE\n"); break;
        case 0xb4: printf("ORA\t\tH\n"); break;
        case 0xb5: printf("ORA\t\tL\n"); break;
        case 0xb6: printf("ORA\t\tM\n"); break;
        case 0xb7: printf("ORA\t\tA\n"); break;
        case 0xb8: printf("CMP\t\tB\n"); break;
        case 0xb9: printf("CMP\t\tC\n"); break;
        case 0xba: printf("CMP\t\tD\n"); break;
        case 0xbb: printf("CMP\t\tE\n"); break;
        case 0xbc: printf("CMP\t\tH\n"); break;
        case 0xbd: printf("CMP\t\tL\n"); break;
        case 0xbe: printf("CMP\t\tM\n"); break;
        case 0xbf: printf("CMP\t\tA\n"); break;
        case 0xc0: printf("RNZ\n"); break;
        case 0xc1: printf("POP\t\tB\n"); break;
        case 0xc2: 
             printf("JNZ\t\t$%02x%02x\n", code[2], code[1]); 
             opbytes = 3;
             break;
        case 0xc3: 
             printf("JMP\t\t$%02x%02x\n", code[2], code[1]); 
             opbytes = 3;
             break;
        case 0xc4:
             printf("CNZ\t\t$%02x%02x\n", code[2], code[1]);
             opbytes = 3;
             break;
        case 0xc5: printf("PUSH\tB\n"); break;
        case 0xc6:
            printf("ADI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xc7: printf("RST 0\n"); break;
        case 0xc8: printf("RZ\n"); break;
        case 0xc9: printf("RET\n"); break;
        case 0xca: 
            printf("JZ\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xcc:
            printf("CZ\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xcd:
            printf("CALL\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xce:
            printf("ACI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xcf: printf("RST\t\t1\n"); break;
        case 0xd0: printf("RNC\n"); break;
        case 0xd1: printf("POP\t\tD\n"); break;
        case 0xd2:
            printf("JNC\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xd3:
            printf("OUT\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xd4:
            printf("CNC\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xd5: printf("PUSH\tD\n"); break;
        case 0xd6:
            printf("SUI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xd7: printf("RST\t\t2\n"); break;
        case 0xd8: printf("RC\n"); break;
        case 0xda:
            printf("JC\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xdb:
            printf("IN\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xdc:
            printf("CC\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xde:
            printf("SBI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xdf: printf("RST\t\t3\n"); break;
        case 0xe0: printf("RPO\n"); break;
        case 0xe1: printf("POP\t\tH\n"); break;
        case 0xe2:
            printf("JPO\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xe3: printf("XTHL\n"); break;
        case 0xe4:
            printf("CPO\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xe5: printf("PUSH\tH\n"); break;
        case 0xe6:
            printf("ANI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xe7: printf("RST\t\t4\n"); break;
        case 0xe8: printf("RPE\n"); break;
        case 0xe9: printf("PCHL\n"); break;
        case 0xea:
            printf("JPE\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xeb: printf("XCHG\n"); break;
        case 0xec:
            printf("CPE\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xee:
            printf("XRI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xef: printf("RST\t\t5\n"); break;
        case 0xf0: printf("RP\n"); break;
        case 0xf1: printf("OP\t\tPSW\n"); break;
        case 0xf2:
            printf("JP\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xf3: printf("DI\n"); break;
        case 0xf4:
            printf("CP\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xf5: printf("PUSH\tPSW\n"); break;
        case 0xf6:
            printf("ORI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xf7: printf("RST\t\t6\n"); break;
        case 0xf8: printf("RM\n"); break;
        case 0xf9: printf("SPHL\n"); break;
        case 0xfa:
            printf("JM\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xfb: printf("EI\n"); break;
        case 0xfc:
            printf("CM\t\t$%02x%02x\n", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xfe:
            printf("CPI\t\t#$%02x\n", code[1]);
            opbytes = 2;
            break;
        case 0xff: printf("RST\t\t7\n"); break;
        default:
            printf("opcode not supported.\n");
            break;
    }

    return opbytes;
}

int main(int argc, char **argv)
{
    int pc = 0;
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }

    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);
    unsigned char *buffer = (unsigned char *)malloc(fsize);
    fread(buffer, fsize, 1, f);
    fclose(f);

    while (pc < fsize) {
        pc += disassemble(buffer, pc);
    }

    free(buffer);

    return 0;
}
