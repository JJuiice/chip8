#include "ins.h"
#include "constants.h"
#include "logging.h"
#include "io.h"
#include <stdio.h>

static void logOp(const char *msg)
{
    #ifndef NDEBUG
    char errMsg[100];
    // char errMsg[2 + sizeof(cpu.opcode.ins) + 8 + sizeof(cpu.PC) + 4 + strlen(msg) + 2];
    sprintf(errMsg, "0x%04X (PC: 0x%04X) - %s", cpu.opcode.ins, cpu.PC, msg);
    logMsg(errMsg);
    #endif
}

void jmp(const uint16_t addr, const char *logMsg)
{  
    logOp(logMsg);
    cpu.PC = addr;
    cpu.PC -= 2;
}

void call(void)
{
    cpu.stack[cpu.SP++] = cpu.PC;
    jmp(cpu.opcode.addr, "0x2NNN: Call NNN");
}

uint8_t loadKeypress(void)
{
    logOp("0xFX0A: Wait for keypress to store in VX");

    cpu.PC -= 2;
    const uint8_t *keyboardState = SDL_GetKeyboardState(NULL);
    for(int i = 0; i < KEY_NUM; i++) {
        if(keyboardState[key_map[i]]) {
            cpu.V[cpu.opcode.x] = i;
            cpu.PC += 2;
            return 0;
        }
    }

    return 1;
}

void draw(void)
{
    logOp("DXYN: draw(VX, VY, N)");
    cpu.V[0xF] = 0;

    for(int y = 0; y < cpu.opcode.n; y++) {
        uint8_t pixel = cpu.mem[cpu.I + y];
        for(int x = 0; x < 8; x++) {
            if(pixel & (0x80 >> x)) {
                uint32_t gfxInx = (cpu.V[cpu.opcode.x] + x) % GFX_WIDTH +
                             ((cpu.V[cpu.opcode.y] + y) % GFX_HEIGHT) *
                             GFX_WIDTH;

                if(gfx[gfxInx] == PIXEL_ON)
                    cpu.V[0xF] = 1;


                gfx[gfxInx] ^= PIXEL_ON;

                if (!cpu.dFlag)
                    cpu.dFlag = 1;
            }
        }
    }
}

void bcd(void)
{
    logOp("0xFX33: Store BCD of VX");
    cpu.mem[cpu.I]     = cpu.V[cpu.opcode.x] / 100;
    cpu.mem[cpu.I + 1] = (cpu.V[cpu.opcode.x] / 10) % 10;
    cpu.mem[cpu.I + 2] = cpu.V[cpu.opcode.x] % 10;
}

void loadMask(uint8_t sb, const char *logMsg)
{
    logOp(logMsg);

    const uint8_t oF = cpu.V[cpu.opcode.x] & sb;
    if(sb == LSB8_MASK) {
        cpu.V[0xF] = oF;
        cpu.V[cpu.opcode.x] >>= 1;
    } else if(sb == MSB8_MASK) {
        cpu.V[0xF] = oF >> 7;
        cpu.V[cpu.opcode.x] <<= 1;
    } else {
        logSDLErrQuit("Inappropriate Bit Mask");
    }
}

void regMemTrans(uint8_t *dst, const uint8_t *src, const uint16_t srcSize, const char *logMsg)
{
    logOp(logMsg);
    const uint16_t iOffset = cpu.I;
           
    if (srcSize == MEM_SIZE)
        memcpy(dst, src + iOffset, cpu.opcode.x + 1);
    else
        memcpy(dst + iOffset, src, cpu.opcode.x + 1);
}

void loadReg(const uint8_t reg, const uint8_t val, const char *logMsg)
{
    logOp(logMsg);

    cpu.V[reg] = val;
}

void loadI(const uint16_t val, const char *logMsg)
{
    logOp(logMsg);

    cpu.I = val;
}

void loadTimer(const uint8_t timer, const uint8_t delay, const char *logMsg)
{
    logOp(logMsg);

    if (timer == DTIMER)
        cpu.dTimer = delay;
    else
        cpu.sTimer = delay;
}

void skipNextIns(uint8_t cond, const char *logMsg)
{
    logOp(logMsg);

    if(cond)
        cpu.PC += 2;
}

void addOF(uint8_t rVal, const uint8_t cond, const char *logMsg)
{
    logOp(logMsg);

    cpu.V[0xF] = cond;
    cpu.V[cpu.opcode.x] += rVal;
}

void dispClear(void)
{
    logOp("0x00E0: Clear display");
    memset(gfx, 0, sizeof(gfx));
    cpu.dFlag = 1;
}

void ret(void)
{
    logOp("0x00EE: RET");
    cpu.PC = cpu.stack[--cpu.SP];
}
