#ifndef INS_H
#define INS_H
#include <stdint.h>
#include "cpu.h"

void jmp();
void call();
void draw();
void storeKeyPInVX();
void bcdVX();
void sbVXInVFLSB(uint8_t sb);
void regMemTrans(uintptr_t *dst, uintptr_t *src);
void set(uintptr_t *lVal, const uint16_t rVal);
void skipNextIns(uint8_t cond);
void addNoCarry(uintptr_t *lVal, const uint16_t rVal);
void addVXWithOverflow(uint8_t rVal, const uint8_t cond);
void dispClear();
void ret();

#endif
