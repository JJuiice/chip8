#include "chip8.h"
#include <stdio.h>

extern char *buffer;

static unsigned short stack[16],
                      op,
                      dFlag,
                      I,
                      PC,
                      SP;

static unsigned char mem[4096],
                     V[16],
                     gfx[64 *32],
                     cKey[16],
                     dTimer,
                     sTimer;

static unsigned char chip8_fontset[80] =
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

void init(void)
{
    PC = 0x200;
    op = 0;
    I = 0;
    SP = 0;

    // clear display
    // clear stak
    // clear reg V0 - VF
    // clear mem

    for(int i = 0; i < 80; ++i)
        mem[i] = chip8_fontset[i];

    // reset timers
}

void loadGame(const char * name)
{
    for(int i = 0; i < sizeof(buffer); ++i)
        mem[i + 512] = buffer[i];
}

void emulateCycle(void)
{
    // Fetch
    op = mem[PC] << 8 | mem[PC + 1];

    // Decode
    switch(op & 0xF000) {
        // Add remaining op cases
        case 0x0000:
            switch(op & 0x00FF) {
                case 0x00E0:
                    break;
                case 0x00EE:
                    break;
                default:
                    printf("Call op", op);
            }
            break;
        case 0x1000:
            break;
        case 0x2000:
            stack[SP] = PC;
            ++SP;
            PC = op & 0x0FFF;
            break;
        case 0x3000:
            break;
        case 0x4000:
            break;
        case 0x5000:
            break;
        case 0x6000:
            break;
        case 0x7000:
            break;
        case 0x8000:
            switch(op & 0x000F) {
                case 0x0000:
                    break;
                case 0x0001:
                    break;
                case 0x0002:
                    break;
                 case 0x0003:
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
                   break;
                case 0x0006:
                   break;
                case 0x0007:
                   break;
                case 0x000E:
                   break;
                default:
                    printf("Undefined opcode [0x8000]: 0x%X\n", op);
            }
            break;
        case 0x9000:
            break;
        case 0xA000:
            I = op & 0x0FFF;
            PC += 2;
            break;
        case 0xB000:
            break;
        case 0xC000:
            break;
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

                        if (!dFlag)
                            dFlag = 1;
                    }
                }
            }

            PC += 2;
            break;
        }
        case 0xE000:
            switch(op & 0x00FF) {
                case 0x009E:
                    if(cKey[V[(op & 0x0F00) >> 8]] != 0)
                        PC += 2;

                    PC += 2;
                    break;
                case 0x00A1:
                    break;
                default:
                    printf("Undefined opcode [0xE000]: 0x%X\n", op);
            }
            break;
        case 0xF000:
            switch(op & 0x00FF) {
                case 0x0007:
                    break;
                case 0x000A:
                    break;
                case 0x0015:
                    break;
                case 0x0018:
                    break;
                case 0x001E:
                    break;
                case 0x0029:
                    break;
                case 0x0033:
                    mem[I]     = V[(op & 0x0F00) >> 8] / 100;
                    mem[I + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
                    mem[I + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;
                    PC += 2;
                    break;
                case 0x0055:
                    break;
                case 0x0065:
                    break;
                default:
                    printf("Undefined opcode [0xF000]: 0x%X\n", op);
            }
            break;
        default:
            printf("Undefined opcode: 0x%X\n", op);
            // Return error?
    }

    if(dTimer > 0)
        --dTimer;

    if(sTimer > 0) {
        if(sTimer == 1)
            printf("BEEP!\n");
        --sTimer;
    }
}

void setKeys(void)
{

}
