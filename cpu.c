#include "constants.h"
#include "cpu.h"
#include "logging.h"
#include "gfx.h"
#include "ins.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

const SDL_Scancode key_map[KEY_NUM] =
{
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

void init(CPU *cpu)
{
    // Reset pointers, opcodes and timers
    cpu->PC = 0x200;
    cpu->opcode.ins = 0;
    cpu->I = 0;
    cpu->SP = 0;
    cpu->dTimer = 0;
    cpu->sTimer = 0;
    cpu->dFlag = 0;

    // clear stack, reg V0 - VF, display, and initialize mem
    memcpy(cpu->mem, fontset, sizeof(fontset));
    memset(cpu->stack, 0, sizeof(cpu->stack));
    memset(cpu->V, 0, sizeof(cpu->V));
    memset(gfx, PIXEL_OFF, sizeof(gfx));

    srand(time(NULL));
}

void loadGame(CPU *cpu, const char *name)
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

    memcpy(&cpu->mem[512], buffer, sizeof(buffer));
}

static void logOpQuit(const char *msg, const uint16_t op)
{
    char errMsg[strlen(msg) + sizeof(uint16_t) + 1];
    sprintf(errMsg, "%s%X", msg, op);
    logQuit(msg);
}

static void logOp(const char *msg, const uint16_t op)
{
    #ifndef NDEBUG
    char errMsg[2 + sizeof(uint16_t) + 3 + strlen(msg) + 2];
    sprintf(errMsg, "0x%X - %s\n", op, msg);
    logErr(msg);
    #endif
}

void emulateCycle(CPU *cpu)
{
    // Fetch
    cpu->opcode.ins = cpu->mem[cpu->PC] << 8 | cpu->mem[cpu->PC + 1];
    uint8_t *vx = &cpu->V[cpu->opcode.x];
    uint8_t *vy = &cpu->V[cpu->opcode.y];

    // Decode
    switch(cpu->opcode.op)
    {
        case 0x0:
        {
            switch(cpu->opcode.addr)
            {
                case 0x0E0: dispClear(cpu->opcode.ins, &cpu->dFlag); break;
                case 0x0EE: ret(cpu); break;
                default:
                    logQuit("MC Subroutines are ignored my modern interpreters");
            }
            break;
        }
        case 0x1: jmp(cpu); break;
        case 0x2: call(cpu); break;
        case 0x3: skipNextIns(cpu, *vx == cpu->opcode.nn); break;
        case 0x4: skipNextIns(cpu, *vx != cpu->opcode.nn); break;
        case 0x5: skipNextIns(cpu, *vx == *vy); break;
        case 0x6: setValUInt8(cpu->opcode.ins, vx, cpu->opcode.nn); break;
        case 0x7: addNoCarry(cpu); break;
        case 0x8:
        {
            switch(cpu->opcode.n)
            {
                case 0x0: setValUInt8(cpu->opcode.ins, vx, *vy); break;
                case 0x1: setValUInt8(cpu->opcode.ins, vx, *vx | *vy); break;
                case 0x2: setValUInt8(cpu->opcode.ins, vx, *vx & *vy); break;
                case 0x3: setValUInt8(cpu->opcode.ins, vx, *vx ^ *vy); break;
                case 0x4: 
                    logOp("0x8XY4: VX += VY (With VF Carry)", cpu->opcode.ins);

                    if(*vy > (0xFF - *vy))
                        cpu->V[0xF] = 1;
                    else
                        cpu->V[0xF] = 0;

                    *vx += *vy;
                    break;
                case 0x5:
                    logOp("0x8XY5: VX -= VY (With VF Borrow)", cpu->opcode.ins);
                    if(*vy > *vy)
                        cpu->V[0xF] = 1;
                    else
                        cpu->V[0xF] = 0;

                    *vx -= *vy;
                    break;
                case 0x6:
                    logOp("0x8XY6: Store VX LSB in VF LSB, then VX>>=1", cpu->opcode.ins);
                    cpu->V[0xF] = *vx & 0x1;
                    *vx >>= 1;
                    break;
                case 0x7:
                    logOp("0x8XY7: VX=VY-VX (with VF carry)", cpu->opcode.ins);
                    if(*vx > *vy)
                        cpu->V[0xF] = 0;
                    else
                        cpu->V[0xF] = 1;

                    *vx = *vy - *vx;
                    break;
                case 0xE:
                {
                    logOp("0x8XYE: Store VX MSB in VF LSB, then VX<<=1", cpu->opcode.ins);
                    cpu->V[0xF] = *vx & 0x80;
                    *vx <<= 1;
                    break;
                }
                default:
                    logOpQuit("Undefined opcode [0x8000]: 0x", cpu->opcode.ins);
            }
            break;
        }
        case 0x9: skipNextIns(cpu, vx != vy); break;
        case 0xA: setValUInt16(cpu->opcode.ins, &cpu->I, cpu->opcode.addr); break;
        case 0xB:
            logOp("0xBNNN: PC=V0+NNN", cpu->opcode.ins);
            cpu->PC = cpu->V[0] + cpu->opcode.addr;
            cpu->PC -= 2;
            break;
        case 0xC:
        {
            logOp("CXNN: VX=rand()&NN", cpu->opcode.ins);
            *vx = (rand() % 0xFF) & cpu->opcode.nn;
            break;
        }
        case 0xD:
        {
            logOp("DXYN: draw(VX, VY, N)", cpu->opcode.ins);
            cpu->V[0xF] = 0;

            for(int yCoord = 0; yCoord < cpu->opcode.n; yCoord++) {
                uint8_t pixel = cpu->mem[cpu->I + yCoord];
                for(int xCoord = 0; xCoord < 8; xCoord++) {
                    if(pixel & (0x80 >> xCoord)) {
                        int gfxInx = (*vx + xCoord) % GFX_WIDTH +
                                     ((*vy + yCoord) % GFX_HEIGHT) *
                                     GFX_WIDTH;

                        if(gfx[gfxInx])
                            cpu->V[0xF] = 1;

                        gfx[gfxInx] ^= ~PIXEL_OFF;

                        if (!cpu->dFlag)
                            cpu->dFlag = 1;
                    }
                }
            }
            break;
        }
        case 0xE:
            switch(cpu->opcode.nn)
            {
                case 0x9E: skipNextIns(cpu, SDL_GetKeyboardState(NULL)[key_map[*vx]]); break;
                case 0xA1: skipNextIns(cpu, !SDL_GetKeyboardState(NULL)[key_map[*vx]]); break;
                default:
                    logOpQuit("Undefined opcode [0xE000]: 0x", cpu->opcode.ins);
            }
            break;
        case 0xF:
            switch(cpu->opcode.nn) {
                case 0x07: setValUInt8(cpu->opcode.ins, vx, cpu->dTimer); break;
                case 0x0A:
                {
                    logOp("0xFX0A: Wait for keypress to store in VX", cpu->opcode.ins);
                    int i;
                    for(i = 0; i < KEY_NUM; i++) {
                        if(SDL_GetKeyboardState(NULL)[key_map[i]]) {
                            *vx = i;
                        }
                    }

                    if(*vx != i)
                        cpu->PC -= 2;

                    break;
                }
                case 0x15: setValUInt8(cpu->opcode.ins, &cpu->dTimer, *vx); break;
                case 0x18: setValUInt8(cpu->opcode.ins, &cpu->sTimer, *vx); break;
                case 0x1E:
                    logOp("0xFX1E: I += VX (No Carry)", cpu->opcode.ins);
                    cpu->I += *vx;
                    break;
                case 0x29: setValUInt16(cpu->opcode.ins, &cpu->I, *vx * 5); break;
                case 0x33:
                    logOp("0xFX33: Store BCD of VX", cpu->opcode.ins);
                    cpu->mem[cpu->I]     = *vx / 100;
                    cpu->mem[cpu->I + 1] = (*vx / 10) % 10;
                    cpu->mem[cpu->I + 2] = (*vx % 100) % 10;
                    break;
                case 0x55:
                {
                    logOp("0xFX55: Store V0 to VX in memory starting at address I", cpu->opcode.ins);
                    uint16_t index = cpu->I;

                    for (int i = 0; i <= *vx; i++) {
                        cpu->mem[index] = cpu->V[i];
                        ++index;
                    }
                    break;
                }
                case 0x65:
                {
                    logOp("0xFX65: Fill V0 to VX in from memory values starting at address I", cpu->opcode.ins);
                    uint16_t index = cpu->I;

                    for (int i = 0; i <= *vx; i++) {
                        cpu->V[i] = cpu->mem[index];
                        ++index;
                    }
                    break;
                }
                default:
                    logOpQuit("Undefined opcode [0xF000]: 0x", cpu->opcode.ins);
            }
            break;
        default:
            logOpQuit("Undefined opcode: 0x", cpu->opcode.ins);
    }

    if(cpu->dTimer > 0)
        --cpu->dTimer;

    if(cpu->sTimer > 0) {
        if(cpu->sTimer == 1)
            printf("BEEP!\n");
        --cpu->sTimer;
    }

    cpu->PC += 2;
}
