#include <stdint.h>
#include <stdio.h>
#include "../globals.h"
#include "../util.h"

void ana(state8080 *state, uint8_t opcode) {
    switch (opcode) {
        case 0xa0:
            if (DEBUG) printf("ANA B\t");
            state->a = state->a & state->b;
            break;
        case 0xa1:
            if (DEBUG) printf("ANA C\t");
            state->a = state->a & state->c;
            break;
        case 0xa2:
            if (DEBUG) printf("ANA D\t");
            state->a = state->a & state->d;
            break;
        case 0xa3:
            if (DEBUG) printf("ANA E\t");
            state->a = state->a & state->e;
            break;
        case 0xa4:
            if (DEBUG) printf("ANA H\t");
            state->a = state->a & state->h;
            break;
        case 0xa5:
            if (DEBUG) printf("ANA L\t");
            state->a = state->a & state->l;
            break;
        case 0xa6:
            if (DEBUG) printf("ANA M\t");
            state->a = state->a & state->memory[getMemOffset(state)];
            break;
        case 0xa7:
            if (DEBUG) printf("ANA A\t");
            state->a = state->a & state->a;
            break;
    }
    setZFlag(state, state->a);
    setSFlag(state, state->a);
    state->cc.cy = 0; //always clear CY
    setPFlag(state, state->a);

    if (DEBUG) printf("ana result: #$%02x\n", state->a);
}

void ani(state8080 *state, uint8_t *opcode) {
    if (DEBUG) printf("ANI $%02x\t", opcode[1]);
    uint8_t answer = state->a & opcode[1];
    setZFlag(state, answer); 
    setSFlag(state, answer);
    setPFlag(state, answer);
    state->cc.cy = 0;
    state->a = answer;
    if (DEBUG) printf("ANI result: $%02x\n", state->a);
}

void xra(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("XRA\t");
    int regno = opcode & 0x07;
    register_kind reg = getRegFromNumber(regno);
    uint8_t regval = getRegVal(state, reg);
    uint8_t answer = state->a ^ regval;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    state->cc.cy = 0;
    state->a = answer;
    if (DEBUG) printf("XRA result: #$%02x\n", state->a);
}

void xri(state8080 *state, uint8_t *opcode) {
    uint8_t imm = opcode[1];
    if (DEBUG) printf("XRI #$%02x\t", imm);
    uint8_t answer = state->a ^ imm;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->a = answer;
    if (DEBUG) printf("XRI result: #$%02x\n", state->a);
}

void ora(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("ORA\t");
    int regno = opcode & 0x07;
    register_kind reg = getRegFromNumber(regno);
    uint8_t regval = getRegVal(state, reg);
    uint8_t answer = state->a | regval;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->a = answer;
    if (DEBUG) printf("ORA result: #$%02x\n", state->a);
}

void ori(state8080 *state, uint8_t *opcode) {
    uint8_t imm = opcode[1];
    if (DEBUG) printf("ORI #$%02x\t", imm);
    uint8_t answer = state->a | imm;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->a = answer;
    if (DEBUG) printf("ORI result: #$%02x\n", state->a);
}

void cmp(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("CMP\t");
    int regno = opcode & 0x07;
    register_kind reg = getRegFromNumber(regno);
    uint8_t regval = getRegVal(state, reg);
    uint8_t answer = state->a - regval;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    setCYFlag(state, state->a, regval, SUB);
}

void cpi(state8080 *state, uint8_t *opcode) {
    uint8_t imm = opcode[1];
    if (DEBUG) printf("CPI #$%02x\n", imm);
    uint8_t answer = state->a - imm;
    setZFlag(state, answer);
    setSFlag(state, answer);
    setPFlag(state, answer);
    setCYFlag(state, state->a, imm, SUB);
}

void rlc(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("RLC\t");
    uint8_t prevHiBit = (state->a & 0x80) >> 7;
    uint8_t answer = state->a << 1;
    answer = answer | prevHiBit;
    state->cc.cy = prevHiBit;
    state->a = answer;
    if (DEBUG) printf("RLC result: #$%02x\n", state->a);
}

void rrc(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("RRC\t");
    uint8_t prevLoBit = (state->a & 0x01);
    uint8_t answer = state->a >> 1;
    answer = (prevLoBit << 7) | answer;
    state->cc.cy = prevLoBit;
    state->a = answer;
    if (DEBUG) printf("RRC result: #$%02x\n", state->a);
}

void ral(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("RAL\t");
    uint8_t prevHiBit = (state->a & 0x80) >> 7;
    uint8_t answer = state->a << 1;
    answer = answer | state->cc.cy;
    state->cc.cy = prevHiBit;
    state->a = answer;
    if (DEBUG) printf("RAL result: #$%02x\n", state->a);
}

void rar(state8080 *state, uint8_t opcode) {
    if (DEBUG) printf("RAR\t");
    uint8_t prevLoBit = state->a & 0x01;
    uint8_t answer = state->a >> 1;
    answer = answer | state->cc.cy;
    state->cc.cy = prevLoBit;
    state->a = answer;
    if (DEBUG) printf("RAR result: #$%02x\n", state->a);
}

void cma(state8080 *state) {
    if (DEBUG) printf("CMA\t");
    uint8_t answer = ~(state->a);
    state->a = answer;
    if (DEBUG) printf("CMA result: #$%02x\n", state->a);
}

void cmc(state8080 *state) {
    if (DEBUG) printf("CMC\t");
    state->cc.cy = ~state->cc.cy;
    if (DEBUG) printf("CMC result: #$%02x\n", state->cc.cy);
}

void stc(state8080 *state) {
    if (DEBUG) printf("STC\t");
    state->cc.cy = 1;
    if (DEBUG) printf("STC result: #$%02x\n", state->cc.cy);
}
