#ifndef INS_H
#define INS_H
#include <stdint.h>
#include "cpu.h"

void jmp(CPU *);
void call(CPU *);
void setValUInt8(uint16_t, uint8_t*, uint8_t);
void setValUInt16(uint16_t, uint16_t*, uint16_t);
void skipNextIns(CPU *, uint8_t);
void addNoCarry(CPU *);
void addVXWithCarry(CPU *, uint8_t);
void dispClear(uint16_t, uint8_t*);
void ret(CPU *);

#endif
