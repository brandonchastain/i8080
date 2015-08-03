#include <stdint.h>
#include <stdio.h>
#include "../globals.h"
#include "../util.h"

void push(state8080* state, uint8_t opcode) {
    if (DEBUG) printf("PUSH\t");
    uint8_t rpNo = (opcode >> 4) & 0x03;
    registerPair_kind rp = getRPFromNumber(state, rpNo);
    if (DEBUG) printf("%s\n", getRPLabel(state, rpNo));
    uint16_t rpVal = getRPVal(state, rp);
    state->memory[state->sp - 1] = (uint8_t)(rpVal >> 8);
    state->memory[state->sp - 2] = (uint8_t)rpVal;
    state->sp -= 2;
}

void pop(state8080* state, uint8_t opcode) {
    if (DEBUG) printf("POP\t");
    uint8_t rpNo = (opcode >> 4) & 0x03;
    registerPair_kind rp = getRPFromNumber(state, rpNo);
    if (DEBUG) printf("%s\n", getRPLabel(state, rpNo));
    uint8_t valHi = state->memory[state->sp + 1];
    uint8_t valLo = state->memory[state->sp];
    setRPVal(state, rp, (valHi << 8) | valLo);
    state->sp += 2;
}

void pushPsw(state8080* state) {
    if (DEBUG) printf("PUSH PSW\n");
    state->memory[state->sp - 1] = state->a;
    uint8_t psw = (state->cc.z |
                    state->cc.s << 1 |
                    state->cc.p << 2 |
                    state->cc.cy << 3 |
                    state->cc.ac << 4 );
    state->memory[state->sp - 2] = psw;
    state->sp -= 2;
}

void popPsw(state8080* state) {
    if (DEBUG) printf("POP PSW\n");
    state->a = state->memory[state->sp + 1];
    uint8_t psw = state->memory[state->sp];
    state->cc.z  = (0x01 == (psw & 0x01));
    state->cc.s  = (0x02 == (psw & 0x02));
    state->cc.p  = (0x04 == (psw & 0x04));
    state->cc.cy = (0x05 == (psw & 0x08));
    state->cc.ac = (0x10 == (psw & 0x10));
    state->sp += 2;
}

void sphl(state8080* state) {
    state->sp = (state->h << 8) | state->l;
}
