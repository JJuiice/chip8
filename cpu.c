#include "constants.h"
#include "cpu.h"
#include "logging.h"
#include "gfx.h"
#include "ins.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

CPU cpu;

const SDL_Scancode key_map[KEY_NUM] =
{
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
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
    memset(gfx, PIXEL_OFF, sizeof(gfx));

    srand(time(NULL));
}

void loadGame(const char *name)
{
    FILE *game = fopen(name, "rb");

    if (game == NULL) {
        const char *err = "Cannot open game file: %s";
        char msg[strlen(err) - 2 + strlen(name) + 1];
        sprintf(msg, err, name);
        logQuit(msg);
    }

    fseek(game, 0L, SEEK_END);
    const size_t BUFFER_SIZE = ftell(game);
    fseek(game, 0L, SEEK_SET);

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    bytesRead = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, game);

    if (bytesRead != BUFFER_SIZE)
        logQuit("Bytes read does not match file size");

    memcpy(&cpu.mem[512], buffer, sizeof(buffer));
}

static void logOpQuit()
{
    char errMsg[20 + sizeof(cpu.opcode.ins) + 5 + sizeof(cpu.opcode.ins) + 1];
    sprintf(errMsg, "Undefined opcode [0x%X]: 0x%X", cpu.opcode.ins & 0xF000, cpu.opcode.ins);
    logQuit(errMsg);
}

void emulateCycle()
{
    // Fetch
    cpu.opcode.ins = cpu.mem[cpu.PC] << 8 | cpu.mem[cpu.PC + 1];
    uintptr_t *vx = (uintptr_t *)(void *) &cpu.V[cpu.opcode.x];
    uint8_t *vy = &cpu.V[cpu.opcode.y];

    fprintf(stderr, "PC: 0x%X, I: 0x%X\n" , cpu.PC, cpu.I);

    // Decode
    switch(cpu.opcode.op)
    {
        case 0x0:
        {
            switch(cpu.opcode.addr)
            {
                case 0x0E0: dispClear(cpu.opcode.ins, &cpu.dFlag); break;
                case 0x0EE: ret(&cpu); break;
                default:
                    logQuit("MC Subroutines are ignored my modern interpreters");
            }
            break;
        }
        case 0x1: jmp(&cpu); break;
        case 0x2: call(&cpu); break;
        case 0x3: skipNextIns(*vx == cpu.opcode.nn); break;
        case 0x4: skipNextIns(*vx != cpu.opcode.nn); break;
        case 0x5: skipNextIns(*vx == *vy); break;
        case 0x6: set(vx, cpu.opcode.nn); break;
        case 0x7: addNoCarry(vx, cpu.opcode.nn); break;
        case 0x8:
        {
            switch(cpu.opcode.n)
            {
                case 0x0: set(vx, *vy); break;
                case 0x1: set(vx, *vx | *vy); break;
                case 0x2: set(vx, *vx & *vy); break;
                case 0x3: set(vx, *vx ^ *vy); break;
                case 0x4: addVXWithOverflow(*vy, *vy > (0xFF - *vx)); break;
                case 0x5: addVXWithOverflow(-(*vy), *vy > *vx); break;
                case 0x6: sbVXInVFLSB(LSB8_MASK); break;
                case 0x7: addVXWithOverflow(*vy - *vx, *vx > *vy); break;
                case 0xE: sbVXInVFLSB(MSB8_MASK); break;
                default:
                    logOpQuit();
            }
            break;
        }
        case 0x9: fprintf(stderr,"*vx: %d, V[x]: %d, *vy: %d, V[y]: %d, *vx != *vy: %d\n", *vx, cpu.V[cpu.opcode.x], *vy, cpu.V[cpu.opcode.y], (*vx != *vy)); skipNextIns(*vx != *vy); break;
        case 0xA: set((uintptr_t *)(void *) &cpu.I, cpu.opcode.addr); break;
        case 0xB: set((uintptr_t *)(void *) &cpu.PC, cpu.V[0]); cpu.PC -= 2; break;
        case 0xC: set(vx, (rand() % 0xFF) & cpu.opcode.nn); break;
        case 0xD: draw(); break;
        case 0xE:
            switch(cpu.opcode.nn)
            {
                case 0x9E: skipNextIns(SDL_GetKeyboardState(NULL)[key_map[*vx]]); break;
                case 0xA1: skipNextIns(~SDL_GetKeyboardState(NULL)[key_map[*vx]]); break;
                default:
                    logOpQuit();
            }
            break;
        case 0xF:
            switch(cpu.opcode.nn) {
                case 0x07: set(vx, cpu.dTimer); break;
                case 0x0A: storeKeyPInVX();
                case 0x15: set((uintptr_t *)(void *) &cpu.dTimer, *vx); break;
                case 0x18: set((uintptr_t *)(void *) &cpu.sTimer, *vx); break;
                case 0x1E: addNoCarry((uintptr_t *)(void *) &cpu.I, *vx); break;
                case 0x29: set((uintptr_t *)(void *) &cpu.I, *vx * 5); break;
                case 0x33: bcdVX(); break;
                case 0x55: regMemTrans((uintptr_t *)(void *) cpu.mem, (uintptr_t *)(void *) cpu.V); break;
                case 0x65: regMemTrans((uintptr_t *)(void *)cpu.V, (uintptr_t *)(void *)cpu.mem); break;
                default:
                    logOpQuit();
            }
            break;
        default:
            logOpQuit();
    }

    if(cpu.dTimer > 0)
        --cpu.dTimer;

    if(cpu.sTimer > 0) {
        if(cpu.sTimer == 1)
            printf("BEEP!\n");
        --cpu.sTimer;
    }

    cpu.PC += 2;
}