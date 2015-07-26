#define DEBUG 1
#define PRINT_DEBUG (DEBUG && !isStepMode)

typedef struct conditionCodes {
	uint8_t z:1; //zero
	uint8_t s:1; //sign, 1 if -, 0 if +
	uint8_t p:1; //parity, 1 if even, 0 if odd
	uint8_t cy:1; //carry
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
	uint16_t memSize;
	struct conditionCodes cc;
	uint8_t int_enable;
} state8080;

typedef enum {ADD, SUB} carry_kind;
typedef enum {BC, DE, HL, SP} registerPair_kind;
typedef enum {A, B, C, D, E, H, L, M} register_kind;

register_kind regs[8];
registerPair_kind rps[4];
uint8_t isStepMode;