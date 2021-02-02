#include "constants.h"
#include "chip8.h"
#include "error_management.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MEM_SIZE 4096
#define STACK_SIZE 16

static unsigned short stack[STACK_SIZE],
                      op,
                      I,
                      PC,
                      SP;

static unsigned char mem[MEM_SIZE],
                     V[16],
                     dTimer,
                     sTimer;

unsigned int drawFlag;
unsigned char gfx[GFX_RESOULTION];
const unsigned char fontset[80] =
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
const SDL_Scancode key_map[KEY_NUM] =
{
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

void init(void)
{
    // Reset pointers, opcodes and timers
    PC = 0x200;
    op = 0;
    I = 0;
    SP = 0;
    dTimer = 0;
    sTimer = 0;
    drawFlag = 0;

    // clear stack, reg V0 - VF, display, and initialize mem
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(gfx, 0, sizeof(gfx));
    memcpy(mem, fontset, sizeof(fontset));

    srand(time(NULL));
}

void loadGame(const char *name)
{
    FILE *game = fopen(name, "rb");

    if (game == NULL) {
        const char *err = "Cannot open game file: %s";
        char msg[strlen(err) - 2 + strlen(name) + 1];
        sprintf(msg, err, name);
        errQuit(msg);
    }

    fseek(game, 0L, SEEK_END);
    const size_t BUFFER_SIZE = ftell(game);
    fseek(game, 0L, SEEK_SET);

    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    bytesRead = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, game);

    if (bytesRead != BUFFER_SIZE)
        errQuit("Bytes read does not match file size");

    memcpy(&mem[512], buffer, sizeof(buffer));
}

static void opErr(char *errMsg, unsigned short op)
{
    char msg[strlen(errMsg) + sizeof(short) + 1];
    const char *err = strcat(errMsg, "%X");
    sprintf(msg, err, op);
    errQuit(msg);
}

void emulateCycle(void)
{
    // Fetch
    op = mem[PC] << 8 | mem[PC + 1];

    // Decode
    switch(op & 0xF000) {
        case 0x0000:
        {
            const unsigned short subOp = op & 0x0FFF;
            switch(subOp) {
                case 0x00E0:
                {
                    int i;
                    for(i = 0; i < GFX_RESOULTION; i++)
                        gfx[i] = 0;
                    drawFlag = 1;
                    break;
                }
                case 0x00EE:
                    --SP; 
                    PC = stack[SP];
                    break;
                default:
                    errQuit("MC Subroutines are ignored my modern interpreters");
            }
            break;
        }
        case 0x1000:
            PC = op & 0x0FFF;
            break;
        case 0x2000:
            stack[SP] = PC;
            ++SP;
            PC = op & 0x0FFF;
            break;
        case 0x3000:
            if(V[(op & 0x0F00) >> 8] == (op & 0x00FF))
                PC += 2;

            PC += 2;
            break;
        case 0x4000:
            if(V[(op & 0x0F00) >> 8] != (op & 0x00FF))
                PC += 2;

            PC += 2;
            break;
        case 0x5000:
            if(V[(op & 0x0F00) >> 8] == V[(op & 0x00F0) >> 4])
                PC += 2;

            PC += 2;
            break;
        case 0x6000:
            V[(op & 0x0F00) >> 8] = (op & 0x00FF);
            PC += 2;
            break;
        case 0x7000:
            V[(op & 0x0F00) >> 8] += (op & 0x00FF);
            PC += 2;
            break;
        case 0x8000:
            switch(op & 0x000F) {
                case 0x0000:
                    V[(op & 0x0F00) >> 8] = V[(op & 0x00F0) >> 4];
                    PC += 2;
                    break;
                case 0x0001:
                    V[(op & 0x0F00) >> 8] = V[(op & 0x0F00) >> 8] | V[(op & 0x00F0) >> 4];
                    PC += 2;
                    break;
                case 0x0002:
                    V[(op & 0x0F00) >> 8] = V[(op & 0x0F00) >> 8] & V[(op & 0x00F0) >> 4];
                    PC += 2;
                    break;
                 case 0x0003:
                    V[(op & 0x0F00) >> 8] = V[(op & 0x0F00) >> 8] ^ V[(op & 0x00F0) >> 4];
                    break;
                case 0x0004:
                    if(V[(op & 0x00F0) >> 4] > (0xFF - V[(op & 0x0F00) >> 8]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[(op & 0x0F00 >> 8)] += V[(op & 0x00F0) >> 4];
                    PC += 2;
                    break;
                case 0x0005:
                    if(V[(op & 0x00F0) >> 4] > V[(op & 0x0F00) >> 8])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[(op & 0x0F00 >> 8)] -= V[(op & 0x00F0) >> 4];
                    PC += 2;
                    break;
                case 0x0006:
                    V[0xF] = V[(op & 0x0100) >> 8];
                    V[(op & 0x0F00) >> 8] >>= 1;
                    PC += 2;
                    break;
                case 0x0007:
                    if(V[(op & 0x0F00) >> 8] > V[(op & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[(op & 0x0F00 >> 8)] = V[(op & 0x00F0) >> 4] - V[(op & 0x0F00) >> 8];
                    PC += 2;
                    break;
                case 0x000E:
                    V[0xF] = V[(op & 0x0800) >> 8];
                    V[(op & 0x0F00) >> 8] <<= 1;
                    PC += 2;
                    break;
                default:
                    opErr("Undefined opcode [0x8000]: 0x", op);
            }
            break;
        case 0x9000:
            if(V[(op & 0x0F00) >> 8] != V[(op & 0x00F0) >> 4])
                PC += 2;

            PC += 2;
            break;
        case 0xA000:
            I = op & 0x0FFF;
            PC += 2;
            break;
        case 0xB000:
            PC = V[0] + (op & 0x0FFF);
            break;
        case 0xC000:
        {
            const unsigned short randNum = rand() % 0xFF;
            V[(op & 0x0F00) >> 8] = randNum & (op & 0x00FF);
            break;
        }
        case 0xD000:
        {
            unsigned short height = op & 0x000F;

            V[0xF] = 0;
            for(int yCord = 0; yCord < height; yCord++) {
                unsigned short sprite = mem[I + yCord];

                for(int xCord = 0; xCord < 8; xCord++) {
                    if((sprite & (0x80 >> xCord)) != 0) {
                        unsigned short x = V[(op & 0x0F00) >> 8],
                                       y = V[(op & 0x00F0) >> 4];

                        if(gfx[x + xCord + ((y + yCord) * 64)] == 1)
                            V[0xF] = 1;

                        gfx[x + xCord + ((y + yCord) * 64)] ^= 1;

                        if (!drawFlag)
                            drawFlag = 1;
                    }
                }
            }

            PC += 2;
            break;
        }
        case 0xE000:
            switch(op & 0x00FF) {
                case 0x009E:
                    if(SDL_GetKeyboardState(NULL)[key_map[V[(op & 0x0F00) >> 8]]])
                        PC += 2;

                    PC += 2;
                    break;
                case 0x00A1:
                    if(!SDL_GetKeyboardState(NULL)[key_map[V[(op & 0x0F00) >> 8]]])
                        PC += 2;

                    PC += 2;
                    break;
                default:
                    opErr("Undefined opcode [0xE000]: 0x", op);
            }
            break;
        case 0xF000:
            switch(op & 0x00FF) {
                case 0x0007:
                    V[(op & 0x0F00) >> 8] = dTimer;
                    PC += 2;
                    break;
                case 0x000A:
                {
                    int i;
                    for(i = 0; i < KEY_NUM; i++) {
                        if(SDL_GetKeyboardState(NULL)[key_map[i]]) {
                            V[(op & 0x0F00) >> 8] = i;
                            PC += 2;
                        }
                    }
                    break;
                }
                case 0x0015:
                    dTimer = V[(op & 0x0F00) >> 8];
                    PC += 2;
                    break;
                case 0x0018:
                    sTimer = V[(op & 0x0F00)];
                    PC += 2;
                    break;
                case 0x001E:
                    I += V[(op & 0x0F00) >> 8];
                    PC += 2;
                    break;
                case 0x0029:
                    I = V[(op & 0x0F00) >> 8] * 5;
                    PC += 2;
                    break;
                case 0x0033:
                    mem[I]     = V[(op & 0x0F00) >> 8] / 100;
                    mem[I + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
                    mem[I + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;
                    PC += 2;
                    break;
                case 0x0055:
                {
                    unsigned short index = I;

                    for (int i = 0; i <= ((op & 0x0F00) >> 8); i++) {
                        mem[index] = V[i];
                        ++index;
                    }

                    PC += 2;
                    break;
                }
                case 0x0065:
                {
                    unsigned short index = I;

                    for (int i = 0; i <= ((op & 0x0F00) >> 8); i++) {
                        V[i] = mem[index];
                        ++index;
                    }

                    PC += 2;
                    break;
                }
                default:
                    opErr("Undefined opcode [0xF000]: 0x", op);
            }
            break;
        default:
            opErr("Undefined opcode: 0x", op);
    }

    if(dTimer > 0)
        --dTimer;

    if(sTimer > 0) {
        if(sTimer == 1)
            printf("BEEP!\n");
        --sTimer;
    }
}
