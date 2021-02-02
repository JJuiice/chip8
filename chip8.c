#include "constants.h"
#include "chip8.h"
#include "error_management.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MEM_SIZE 4096
#define STACK_SIZE 16

static uint16_t stack[STACK_SIZE];
static uint16_t op;
static uint16_t I;
static uint16_t PC;
static uint16_t SP;

static uint8_t mem[MEM_SIZE];
static uint8_t V[16];

static uint8_t dTimer;
static uint8_t sTimer;

uint8_t drawFlag;
uint32_t gfx[GFX_RESOULTION];
const uint8_t fontset[80] =
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
    memset(gfx, PIXEL_OFF, sizeof(gfx));
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

    uint8_t buffer[BUFFER_SIZE];
    size_t bytesRead = 0;

    bytesRead = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, game);

    if (bytesRead != BUFFER_SIZE)
        errQuit("Bytes read does not match file size");

    memcpy(&mem[512], buffer, sizeof(buffer));
}

static void opErr(char *errMsg)
{
    char msg[strlen(errMsg) + sizeof(short) + 1];
    const char *err = strcat(errMsg, "%X");
    sprintf(msg, err, op);
    errQuit(msg);
}

static void logOps(char *msg)
{
    #ifndef NDEBUG
    fprintf(stderr, "0x%X - %s\n", op, msg);
    #endif
}

void emulateCycle(void)
{
    // Fetch
    op = mem[PC] << 8 | mem[PC + 1];

    // Decode
    switch(op & 0xF000) {
        case 0x0000:
        {
            const uint16_t subOp = op & 0x0FFF;
            switch(subOp) {
                case 0x00E0:
                {
                    logOps("0x00E0: Clear display");
                    memset(gfx, 0, sizeof(gfx));
                    drawFlag = 1;
                    PC += 2;
                    break;
                }
                case 0x00EE:
                    logOps("0x00EE: RET");
                    --SP; 
                    PC = stack[SP];
                    break;
                default:
                    errQuit("MC Subroutines are ignored my modern interpreters");
            }
            break;
        }
        case 0x1000:
            logOps("0x1NNN: JMP NNN");
            PC = op & 0x0FFF;
            break;
        case 0x2000:
            logOps("0x2NNN: Call NNN");
            stack[SP] = PC;
            ++SP;
            PC = op & 0x0FFF;
            break;
        case 0x3000:
            logOps("0x2XNN: Skip next instruction if VX == NN");
            if(V[(op & 0x0F00) >> 8] == (op & 0x00FF))
                PC += 2;

            PC += 2;
            break;
        case 0x4000:
            logOps("0x4XNN: Skip next instruction if VX != NN");
            if(V[(op & 0x0F00) >> 8] != (op & 0x00FF))
                PC += 2;

            PC += 2;
            break;
        case 0x5000:
            logOps("0x5XY0: Skip next intruction if VX == VY");
            if(V[(op & 0x0F00) >> 8] == V[(op & 0x00F0) >> 4])
                PC += 2;

            PC += 2;
            break;
        case 0x6000:
            logOps("0x6XNN: VX = NN");
            V[(op & 0x0F00) >> 8] = (op & 0x00FF);
            PC += 2;
            break;
        case 0x7000:
            logOps("0x7XNN: VX += NN (No Carry)");
            V[(op & 0x0F00) >> 8] += (op & 0x00FF);
            PC += 2;
            break;
        case 0x8000:
        {
            const uint16_t x = (op & 0x0F00) >> 8;
            const uint16_t y = (op & 0x00F0) >> 4;
            
            switch(op & 0x000F)
            {
                case 0x0000:
                    logOps("0x8XY0: VX = VY");
                    V[x] = V[y];
                    break;
                case 0x0001:
                    logOps("0x8XY1: VX = VX | VY");
                    V[x] = V[x] | V[y];
                    break;
                case 0x0002:
                    logOps("0x8XY2: VX = VX & VY");
                    V[x] = V[x] & V[y];
                    break;
                 case 0x0003:
                    logOps("0x8XY3: VX = VX ^ VY");
                    V[x] = V[x] ^ V[y];
                    break;
                case 0x0004:
                    logOps("0x8XY4: VX += VY (With VF Carry Flag)");
                    if(V[y] > (0xFF - V[x]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[x] += V[y];
                    break;
                case 0x0005:
                    logOps("0x8XY5: VX-=VY (With VF Carry Flag)");
                    if(V[y] > V[x])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    V[x] -= V[y];
                    break;
                case 0x0006:
                    logOps("0x8XY6: Store VX LSB in VF LSB, then VX>>=1");
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                    break;
                case 0x0007:
                    logOps("0x8XY7: VX=VY-VX (with VF carry)");
                    if(V[x] > V[y])
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;

                    V[x] = V[y] - V[x];
                    break;
                case 0x000E:
                {
                    logOps("0x8XYE: Store VX MSB in VF LSB, then VX<<=1");
                    V[0xF] = V[x] & 0x80;
                    V[x] <<= 1;
                    break;
                }
                default:
                    opErr("Undefined opcode [0x8000]: 0x");
            }
            
            PC += 2;
            break;
        }
        case 0x9000:
            logOps("0x9XY0: Skip next intruction if VX != VY");
            if(V[(op & 0x0F00) >> 8] != V[(op & 0x00F0) >> 4])
                PC += 2;

            PC += 2;
            break;
        case 0xA000:
            logOps("0xANNN: I = NNN");
            I = op & 0x0FFF;
            PC += 2;
            break;
        case 0xB000:
            logOps("0xBNNN: PC=V0+NNN");
            PC = V[0] + (op & 0x0FFF);
            break;
        case 0xC000:
        {
            logOps("CXNN: VX=rand()&NN");
            const uint8_t randNum = rand() % 0xFF;
            V[(op & 0x0F00) >> 8] = randNum & (op & 0x00FF);
            PC += 2;
            break;
        }
        case 0xD000:
        {
            logOps("DXYN: draw(VX, VY, N)");
            uint8_t x = V[(op & 0x0F00) >> 8];
            uint8_t y = V[(op & 0x00F0) >> 4];
            uint8_t n = op & 0x000F;
            V[0xF] = 0;

            for(int yCoord = 0; yCoord < n; yCoord++) {
                uint8_t pixel = mem[I + yCoord];
                for(int xCoord = 0; xCoord < 8; xCoord++) {
                    if(pixel & (0x80 >> xCoord)) {
                        int gfxInx = (x + xCoord) % GFX_WIDTH +
                                     ((y + yCoord) % GFX_HEIGHT) *
                                     GFX_WIDTH;

                        if(gfx[gfxInx])
                            V[0xF] = 1;

                        gfx[gfxInx] ^= ~PIXEL_OFF;

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
                    logOps("0xEX9E: Skip next instruction if key()==VX");
                    if(SDL_GetKeyboardState(NULL)[key_map[V[(op & 0x0F00) >> 8]]])
                        PC += 2;

                    PC += 2;
                    break;
                case 0x00A1:
                    logOps("0xEXA1: Skip next instruction if key()!=VX");
                    if(!SDL_GetKeyboardState(NULL)[key_map[V[(op & 0x0F00) >> 8]]])
                        PC += 2;

                    PC += 2;
                    break;
                default:
                    opErr("Undefined opcode [0xE000]: 0x");
            }
            break;
        case 0xF000:
            switch(op & 0x00FF) {
                case 0x0007:
                    logOps("0xFX07: VX = dTimer");
                    V[(op & 0x0F00) >> 8] = dTimer;
                    PC += 2;
                    break;
                case 0x000A:
                {
                    logOps("0xFX0A: Wait for keypress to store in VX");
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
                    logOps("0xFX15: dTimer = VX");
                    dTimer = V[(op & 0x0F00) >> 8];
                    PC += 2;
                    break;
                case 0x0018:
                    logOps("0xFX18: sTimer = VX");
                    sTimer = V[(op & 0x0F00)];
                    PC += 2;
                    break;
                case 0x001E:
                    logOps("0xFX1E: I += VX (No Carry)");
                    I += V[(op & 0x0F00) >> 8];
                    PC += 2;
                    break;
                case 0x0029:
                    logOps("0xFX29E: I = sprite_addr[VX]");
                    I = V[(op & 0x0F00) >> 8] * 5;
                    PC += 2;
                    break;
                case 0x0033:
                    logOps("0xFX33: Store BCD of VX");
                    mem[I]     = V[(op & 0x0F00) >> 8] / 100;
                    mem[I + 1] = (V[(op & 0x0F00) >> 8] / 10) % 10;
                    mem[I + 2] = (V[(op & 0x0F00) >> 8] % 100) % 10;
                    PC += 2;
                    break;
                case 0x0055:
                {
                    logOps("0xFX55: Store V0 to VX in memory starting at address I");
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
                    logOps("0xFX65: Fill V0 to VX in from memory values starting at address I");
                    unsigned short index = I;

                    for (int i = 0; i <= ((op & 0x0F00) >> 8); i++) {
                        V[i] = mem[index];
                        ++index;
                    }

                    PC += 2;
                    break;
                }
                default:
                    opErr("Undefined opcode [0xF000]: 0x");
            }
            break;
        default:
            opErr("Undefined opcode: 0x");
    }

    if(dTimer > 0)
        --dTimer;

    if(sTimer > 0) {
        if(sTimer == 1)
            printf("BEEP!\n");
        --sTimer;
    }
}
