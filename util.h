//printing
void printFlags(state8080*);
void debugPrint(state8080*);
void printMem(state8080*);

//condition flags
void setZFlag(state8080*, uint16_t);
void setSFlag(state8080*, uint16_t);
void setCYFlag(state8080*, uint8_t, uint8_t, carry_kind);
void setPFlag(state8080*, uint16_t);

//utility
uint16_t getMemOffset(state8080*);
register_kind getRegFromNumber(uint8_t);
registerPair_kind getRPFromNumber(state8080*, uint8_t);
uint8_t getRegVal(state8080*, register_kind);
void setRegVal(state8080*, register_kind, uint8_t);
char* getRegLabel(state8080*, register_kind);
uint16_t getRPVal(state8080*, registerPair_kind);
void setRPVal(state8080*, registerPair_kind, uint16_t);
char* getRPLabel(state8080*, registerPair_kind);

void unimplementedInstr(state8080*);
void invalidInstr(state8080*);