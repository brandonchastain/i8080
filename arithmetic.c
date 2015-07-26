#include <stdint.h>
#include <stdio.h>
#include "globals.h"
#include "util.h"

void addToA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a + (uint16_t)value;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, ADD);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Add result: $%02x\t", state->a);
}

void adcToA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a + (uint16_t)value;
	answer += state->cc.cy;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, ADD);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Adc result: $%02x\t", state->a);
}

void subFromA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a - (uint16_t)value;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setPFlag(state, answer);
	setCYFlag(state, state->a, value, SUB);
	state->a = answer & 0xff;
	if(DEBUG) printf("SUB result: $%02x\t", state->a);
}

void sbbFromA(state8080 *state, uint8_t value){
	uint16_t answer = (uint16_t)state->a - (uint16_t)value;
	answer -= (uint16_t)state->cc.cy;
	setZFlag(state, answer);
	setSFlag(state, answer);
	setCYFlag(state, state->a, value, SUB);
	setPFlag(state, answer);
	state->a = answer & 0xff;
	if(DEBUG) printf("Sbb result: $%02x\t", state->a);
}

void decrRP(state8080 *state, registerPair_kind rp){
	uint8_t *rh, *rl;

	if (rp == SP) {
		uint8_t hi = (uint8_t)((state->sp & 0xff00) >> 8);
		uint8_t lo = (uint8_t)(state->sp & 0x00ff);	
		rh = &hi;
		rl = &lo;
	} else {
		switch(rp) {
			case BC: rh = &state->b; rl = &state->c; break;
			case DE: rh = &state->d; rl = &state->e; break;
			case HL: rh = &state->h; rl = &state->l; break;
			case SP: break;
		}
	}

	uint16_t temp = (uint16_t)*rl;
	if (temp == 0x00) {
		*rh -= 1;
	}
	*rl = (*rl - 1) & 0xff;
}

void addRPtoHL(state8080 *state, registerPair_kind rp) {
	uint32_t rp1;
	uint32_t rp2 = (state->h << 8) | state->l;
	switch(rp) {
		case BC:
			rp1 = (state->b << 8) | state->c;
			break;
		case DE:
			rp1 = (state->d << 8) | state->e;
			break;
		case HL:
		default:
			rp1 = (state->h << 8) | state->l;
			break;
	}
	uint32_t answer = rp1 + rp2;
	state->cc.cy = answer > 0xffff;
	state->h = (answer >> 8) & 0xff;
	state->l = answer & 0xff;
}

void daa(state8080 *state) {
    if ((state->a & 0x0f) > 9 || state->cc.ac) {
        state->a += 6;
    }
    if ((state->a >> 4) > 9 || state->cc.cy) {
        uint16_t res = (uint16_t) state->a + 0x60;
        setZFlag(state, res);
        setSFlag(state, res);
        setCYFlag(state, state->a, 0x60, ADD);
        setPFlag(state, res);
        state->a = res & 0xff;
    }
}