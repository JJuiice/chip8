#ifndef INS_H
#define INS_H
#include <stdint.h>
#include "cpu.h"

void jmp(const uint16_t addr, const char *logMsg);
void call(void);
void draw(void);
void storeKeyPInVX(void);
void bcdXReg(void);
void sbXRegInOFLSB(uint8_t sb, const char *logMsg);
void regMemTrans(uint8_t *dst, const uint8_t *src, const uint16_t srcSize, const char *logMsg);
void setReg(const uint8_t reg, const uint8_t val, const char *logMsg);
void setI(const uint16_t val, const char *logMsg);
void setTimer(const uint8_t timer, const uint8_t delay, const char *logMsg);
void skipNextIns(uint8_t cond, const char *logMsg);
void addXRegWithOverflow(uint8_t rVal, const uint8_t cond, const char *logMsg);
void dispClear();
void ret();

#endif
