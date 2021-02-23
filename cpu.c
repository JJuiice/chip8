/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#include "constants.h"
#include "cpu.h"
#include "logging.h"
#include "io.h"
#include "ins.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define DEBOUNCE_MS 150 

CPU cpu;

static uint32_t debounceSTick;
static const uint8_t fontset[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

void init()
{
    // Reset pointers, opcodes and timers
    cpu.PC = 0x200;
    cpu.opcode.ins = 0;
    cpu.I = 0;
    cpu.SP = 0;
    cpu.dTimer = 0;
    cpu.sTimer = 0;
    cpu.dFlag = 0;

    // clear stack, reg V0 - VF, display, and initialize mem
    memcpy(cpu.mem, fontset, sizeof(fontset));
    memset(cpu.stack, 0, sizeof(cpu.stack));
    memset(cpu.V, 0, sizeof(cpu.V));
    clrGfx();

    debounceSTick = 0;
    srand(time(NULL));
}

void loadGame(const char *name)
{
    FILE *game = fopen(name, "rb");

    if (game == NULL) {
        const char *err = "CANNOT OPEN GAME FILE: %s";
        char msg[strlen(err) - 2 + strlen(name) + 1];
        sprintf(msg, err, name);
        logSDLErrQuit(msg);
    }

    fseek(game, 0L, SEEK_END);
    const size_t BUFFER_SIZE = ftell(game);
    fseek(game, 0L, SEEK_SET);

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    bytesRead = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, game);

    if (bytesRead != BUFFER_SIZE)
        logSDLErrQuit("BYTES READ DO NOT MATCH FILE SIZE");

    memcpy(&cpu.mem[512], buffer, sizeof(buffer));
}

void updateTimers() {
    if(cpu.dTimer > 0)
        cpu.dTimer--;

    if(cpu.sTimer > 0) {
        if(isAudioPaused())
            pauseAudio(0);

        if((cpu.sTimer--) == 1)
            pauseAudio(1);

        checkSDLErr(__LINE__);
    }
}

static uint8_t debouncing()
{
    uint8_t isDebouncing = 0;

    if(debounceSTick > 0)
        if(getSDLTimestamp() - debounceSTick >= DEBOUNCE_MS) {
            debounceSTick = 0;
        } else {
            isDebouncing = 1;
            cpu.PC -= 2;
        }
        
    return isDebouncing;
}

static void setDebounce()
{
    debounceSTick = getSDLTimestamp();
}

static void logOpQuit()
{
    char errMsg[50];
    sprintf(errMsg, "UNDEFINED OPCODE [0x%04X]: 0x%04X", cpu.opcode.ins & 0xF000, cpu.opcode.ins);
    logSDLErrQuit(errMsg);
}

void emulateCycle()
{
    // Fetch
    cpu.opcode.ins = cpu.mem[cpu.PC] << 8 | cpu.mem[cpu.PC + 1];
    uint8_t vx = cpu.V[cpu.opcode.x];
    uint8_t vy = cpu.V[cpu.opcode.y];

    // Decode
    switch(cpu.opcode.op)
    {
        case 0x0:
        {
            switch(cpu.opcode.addr)
            {
                case 0x0E0: dispClear(); break;
                case 0x0EE: ret(); break;
                default:
                    logSDLErrQuit("MC SUBROUTINES ARE IGNORED BY MODERN INTERPRETERS");
            }
            break;
        }
        case 0x1: jmp(cpu.opcode.addr, "0x1NNN: JP NNN"); break;
        case 0x2: call(); break;
        case 0x3: skipNextIns(vx == cpu.opcode.kk, "0x3XKK: VX, KK"); break;
        case 0x4: skipNextIns(vx != cpu.opcode.kk, "0x4XKK: SNE VX, KK"); break;
        case 0x5: skipNextIns(vx == vy, "0x5XY0: SE VX, VY"); break;
        case 0x6: loadReg(cpu.opcode.x, cpu.opcode.kk, "0x6XKK: LD VX, KK"); break;
        case 0x7: loadReg(cpu.opcode.x, vx + cpu.opcode.kk, "0x7XKK: ADD VX, KK (NO CARRY)"); break;
        case 0x8:
        {
            switch(cpu.opcode.n)
            {
                case 0x0: loadReg(cpu.opcode.x, vy, "0x8XY0: LD VX, VY"); break;
                case 0x1: loadReg(cpu.opcode.x, vx | vy, "0x8XY0: OR VX, VY"); break;
                case 0x2: loadReg(cpu.opcode.x, vx & vy, "0x8XY2: AND VX, VY"); break;
                case 0x3: loadReg(cpu.opcode.x, vx ^ vy, "0x8XY3: XOR VX, VY"); break;
                case 0x4: addOF(vy, (vy + vx) > 0xFF, "0x8XY4: ADD VX, VY (WITH CARRY)"); break;
                case 0x5: addOF(-vy, vx > vy, "0x8XY5: SUB VX, VY (WITH BORROW)"); break;
                case 0x6: loadMask(LSB8_MASK, "0x8XY6: SHR VX {, VY}"); break;
                case 0x7: addOF(vy - vx, vx < vy, "0x8XY7: SUBN VX, VY"); break;
                case 0xE: loadMask(MSB8_MASK, "0x8XYE: SHL VX {, VY}"); break;
                default:
                    logOpQuit();
            }
            break;
        }
        case 0x9: skipNextIns(vx != vy, "0x9XYN: SNE VX, VY"); break;
        case 0xA: loadI(cpu.opcode.addr, "0xANNN: LD I, NNN"); break;
        case 0xB: jmp(cpu.V[0] + cpu.opcode.addr, "0xBNNN: JP V0, NNN"); break;
        case 0xC: loadReg(cpu.opcode.x, (rand() % 0xFF) & cpu.opcode.kk, "0xCXKK: RND VX, KK"); break;
        case 0xD: draw(); break;
        case 0xE:
            switch(cpu.opcode.kk)
            {
                case 0x9E: skipNextIns(isKeyPressed(getKeyboardState(), vx), "0xEX93: SKP VX"); break;
                case 0xA1: skipNextIns(!isKeyPressed(getKeyboardState(), vx), "0xEXA1: SKNP VX"); break;
                default:
                    logOpQuit();
            }
            break;
        case 0xF:
            switch(cpu.opcode.kk) {
                case 0x07: loadReg(cpu.opcode.x, cpu.dTimer, "0xFX07: LD VX, DT"); break;
                case 0x0A: if(!debouncing() && !loadKeypress()) setDebounce(); break;
                case 0x15: loadTimer(DTIMER, vx, "0xFX15: LD DT, VX"); break;
                case 0x18: loadTimer(STIMER, vx, "0xFX18: LD ST, VX"); break;
                case 0x1E: loadI(cpu.I + vx, "0xFX1E: ADD I, VX (NO CARRY)"); break;
                case 0x29: loadI(vx * 5, "0xFX29: LD I, SPRITE_ADDR(VX)"); break;
                case 0x33: bcd(); break;
                case 0x55: regMemTrans(cpu.mem, cpu.V, 16, "0xFX55: LD [I], V0 (UNTIL VX)"); break;
                case 0x65: regMemTrans(cpu.V, cpu.mem, MEM_SIZE, "0xFX65: LD V0, [I] (UNTIL VX)"); break;
                default:
                    logOpQuit();
            }
            break;
        default:
            logOpQuit();
    }

    cpu.PC += 2;
    #ifndef NDEBUG
    dumpRegAndPointerInfo();
    #endif 
} 
