/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#ifndef INS_H
#define INS_H
#include <stdint.h>

void jmp(const uint16_t addr, const char *logMsg);
void call(void);
void draw(void);
void bcd(void);
void regMemTrans(uint8_t *dst, const uint8_t *src, const uint16_t srcSize, const char *logMsg);
void loadReg(const uint8_t reg, const uint8_t val, const char *logMsg);
void loadI(const uint16_t val, const char *logMsg);
void loadTimer(const uint8_t timer, const uint8_t delay, const char *logMsg);
void skipNextIns(uint8_t cond, const char *logMsg);
void addOF(uint8_t rVal, const uint8_t cond, const char *logMsg);
void dispClear(void);
void ret(void);

int loadKeypress(void);
int loadMask(uint8_t sb, const char *logMsg);

#endif
