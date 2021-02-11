/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#ifndef CPU_H
#define CPU_H
#include <stdint.h>

#define MEM_SIZE 4096
#define STACK_SIZE 16

#define STIMER 2 
#define DTIMER 4

union Opcode
{
    uint16_t ins;
    struct
    {
        uint8_t n  : 4;
        uint8_t y  : 4;
        uint8_t x  : 4;
        uint8_t op : 4;
    };
    struct
    {
        uint8_t kk : 8;
    };
    struct
    {
        uint16_t addr : 12;
    };
};

typedef struct CPU
{
    union Opcode opcode;

    uint16_t I;
    uint16_t PC;
    uint16_t SP;
    uint16_t stack[STACK_SIZE];

    uint8_t mem[MEM_SIZE];
    uint8_t V[16];

    uint8_t dTimer;
    uint8_t sTimer;

    uint8_t dFlag;
} CPU;

void init();
void loadGame(const char*);
void emulateCycle();
void updateTimers();

extern CPU cpu;

#endif
