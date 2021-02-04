#ifndef INS_H
#define INS_H
#include <stdint.h>
#include "cpu.h"

void jmp(const uint16_t addr);
void call(void);
void draw(void);
void storeKeyPInVX(void);
void bcdVX(void);
void sbVXInVFLSB(uint8_t sb);
void regMemTrans(uint8_t *dst, const uint8_t *src, const uint16_t srcSize);
void setReg(const uint8_t reg, const uint8_t val);
void setI(const uint16_t val);
void setTimer(const uint8_t timer, const uint8_t delay);
void skipNextIns(uint8_t cond);
void addNoCarryVX(const uint8_t rVal);
void addNoCarryI(const uint16_t rVal);
void addVXWithOverflow(uint8_t rVal, const uint8_t cond);
void dispClear();
void ret();

#endif
