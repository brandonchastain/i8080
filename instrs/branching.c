#include <stdint.h>
#include "../globals.h"
#include "../util.h"

void jmp(state8080 *state, uint8_t *opcode) {
    state->pc = (opcode[2] << 8) | (opcode[1]);
}

void call(state8080 *state, uint8_t *opcode) {
#ifdef FOR_CPUDAIG
    if (5 == ((opcode[2] << 8) | opcode[1])){
        if (state->c == 9) {
            uint16_t offest = (state->d << 8) | state->e;
            char *str = &state->memory[offset+3];
            while (*str != '$')
                printf("%c", *str++);
            printf("\n");
        } else if (state->c == 2) {
            printf("print char routine called\n");
        }
    } else if (0 == ((opcode[2] << 8) | opcode[1])){
        exit(0);
    } else 
#endif 
    {
        uint16_t ret = state->pc + 2;
        state->memory[state->sp-1] = (ret >> 8) & 0xff;
        state->memory[state->sp-2] = ret & 0xff;
        state->sp -= 2;
        jmp(state, opcode);
    }
}

void ret(state8080 *state) {
    uint8_t retAddrLo = state->memory[state->sp];
    uint8_t retAddrHi = state->memory[state->sp+1];
    state->pc = (retAddrHi << 8) | retAddrLo;
    state->pc += 2;
}

void rst(state8080 *state, uint8_t num) {
    state->memory[state->sp-1] = (state->pc >> 8) & 0xff;
    state->memory[state->sp-2] = (state->pc) & 0xff;
    state->sp -= 2;
    //multiply by 8.... probably a pre-optimization
    state->pc = num << 3 & 0x0038;
}

void pchl(state8080 *state) {
    state->pc = getMemOffset(state);
}
