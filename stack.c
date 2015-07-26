#include <stdint.h>
#include <stdio.h>
#include "globals.h"
#include "util.h"

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

void sphl(state8080* state) {
    state->sp = (state->h << 8) | state->l;
}