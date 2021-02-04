#include "ins.h"
#include "constants.h"
#include "logging.h"
#include <stdio.h>

static void logOp(const char *msg)
{
    #ifndef NDEBUG
    char errMsg[100];
    // char errMsg[2 + sizeof(cpu.opcode.ins) + 8 + sizeof(cpu.PC) + 4 + strlen(msg) + 2];
    sprintf(errMsg, "0x%X (PC: 0x%X) - %s", cpu.opcode.ins, cpu.PC, msg);
    logErr(errMsg);
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
    logOp("0x2NNN: Call NNN");
    cpu.stack[cpu.SP] = cpu.PC;
    cpu.SP++;
    cpu.PC = cpu.opcode.addr;
    cpu.PC -= 2;
}

void storeKeyPInVX(void)
{
    logOp("0xFX0A: Wait for keypress to store in VX");

    uint8_t *vx = &cpu.V[cpu.opcode.x];

    int i;
    for(i = 0; i < KEY_NUM; i++) {
        if(SDL_GetKeyboardState(NULL)[key_map[i]]) {
            *vx = i;
        }
    }

    if(*vx != i)
        cpu.PC -= 2;
}

void draw(void)
{
    logOp("DXYN: draw(VX, VY, N)");
    cpu.V[0xF] = 0;

    for(int y = 0; y < cpu.opcode.n; y++) {
        uint8_t pixel = cpu.mem[cpu.I + y];
        for(int x = 0; x < 8; x++) {
            if(pixel & (0x80 >> x)) {
                int gfxInx = (cpu.V[cpu.opcode.x] + x) % GFX_WIDTH +
                             ((cpu.V[cpu.opcode.y] + y) % GFX_HEIGHT) *
                             GFX_WIDTH;

                if(gfx[gfxInx])
                    cpu.V[0xF] = 1;

                gfx[gfxInx] ^= ~PIXEL_OFF;

                if (!cpu.dFlag)
                    cpu.dFlag = 1;
            }
        }
    }
}

void bcdVX(void)
{
    logOp("0xFX33: Store BCD of VX");
    cpu.mem[cpu.I]     = cpu.V[cpu.opcode.x] / 100;
    cpu.mem[cpu.I + 1] = (cpu.V[cpu.opcode.x] / 10) % 10;
    cpu.mem[cpu.I + 2] = (cpu.V[cpu.opcode.x] % 100) % 10;
}

void sbVXInVFLSB(uint8_t sb, const char *logMsg)
{
    logOp(logMsg);

    cpu.V[0xF] = cpu.V[cpu.opcode.x] & sb;
    if(sb == LSB8_MASK) {
        cpu.V[cpu.opcode.x] >>= 1;
    } else if(sb == MSB8_MASK) {
        cpu.V[cpu.opcode.x] <<= 1;
    } else {
        logQuit("Inappropriate Bit Mask");
    }
}

void regMemTrans(uint8_t *dst, const uint8_t *src, const uint16_t srcSize, const char *logMsg)
{
    logOp(logMsg);
           
    if (srcSize == MEM_SIZE)
        memcpy(dst, &src[cpu.I], sizeof(uint8_t) * srcSize);
    else
        memcpy(&dst[cpu.I], src, sizeof(uint8_t) * srcSize);
}

void setReg(const uint8_t reg, const uint8_t val, const char *logMsg)
{
    logOp(logMsg);

    cpu.V[reg] = val;
}

void setI(const uint16_t val, const char *logMsg)
{
    logOp(logMsg);

    cpu.I = val;
}

void setTimer(const uint8_t timer, const uint8_t delay, const char *logMsg)
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

void addVXWithOverflow(uint8_t rVal, const uint8_t cond, const char *logMsg)
{
    logOp(logMsg);

    if(cond)
        cpu.V[0xF] = 1;
    else
        cpu.V[0xF] = 0;
    
    cpu.V[cpu.opcode.x] = rVal;
}

void dispClear()
{
    logOp("0x00E0: Clear display");
    memset(gfx, 0, sizeof(gfx));
    cpu.dFlag = 1;
}

void ret()
{
    logOp("0x00EE: RET");
    cpu.PC = cpu.stack[--cpu.SP];
    cpu.PC -= 2;
}
