#include "ins.h"
#include "constants.h"
#include "logging.h"
#include <stdio.h>

static void logOp(const char *msg)
{
    #ifndef NDEBUG
    char errMsg[2 + sizeof(cpu.opcode.ins) + 3 + strlen(msg) + 2];
    sprintf(errMsg, "0x%X - %s\n", cpu.opcode.ins, msg);
    logErr(errMsg);
    #endif
}

void jmp()
{  
    logOp("0x1NNN: JMP NNN");
    cpu.PC = cpu.opcode.addr;
    cpu.PC -= 2;
}

void call()
{
    logOp("0x2NNN: Call NNN");
    cpu.stack[cpu.SP] = cpu.PC;
    ++cpu.SP;
    cpu.PC = cpu.opcode.addr;
    cpu.PC -= 2;
}

void storeKeyPInVX()
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

void draw()
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

void bcdVX()
{
    logOp("0xFX33: Store BCD of VX");
    cpu.mem[cpu.I]     = cpu.V[cpu.opcode.x] / 100;
    cpu.mem[cpu.I + 1] = (cpu.V[cpu.opcode.x] / 10) % 10;
    cpu.mem[cpu.I + 2] = (cpu.V[cpu.opcode.x] % 100) % 10;
}

void sbVXInVFLSB(uint8_t sb)
{
    logOp("0x8XY6: Store VX LSB in VF LSB, then VX>>=1"
          "\t 0x8XYE: Store VX MSB in VF LSB, then VX<<=1");

    cpu.V[0xF] = cpu.V[cpu.opcode.x] & sb;
    if(sb == LSB8_MASK) {
        cpu.V[cpu.opcode.x] >>= 1;
    } else if(sb == MSB8_MASK) {
        cpu.V[cpu.opcode.x] <<= 1;
    } else {
        logQuit("Inappropriate Bit Mask");
    }
}

void regMemTrans(uintptr_t *dst, uintptr_t *src)
{
    logOp("0xFX55: Store V0 to VX in memory starting at address I\n"
          "\t 0xFX65: Fill V0 to VX in from memory values starting at address I");

    uint16_t inx = cpu.I;
    
    for(int i = 0; i <= cpu.V[cpu.opcode.x]; i++)
        dst[i] = src[inx++];
}

void set(uintptr_t *lVal, const uint16_t rVal)
{
    logOp("0x6XNN: VX = NN\n"
          "\t 0x8XY0: VX = VY\n"
          "\t 0x8XY1: VX = VX | VY\n"
          "\t 0x8XY2: VX = VX & VY\n"
          "\t 0x8XY3: VX = VX ^ VY\n"
          "\t 0xANNN: I = NNN\n"
          "\t 0xBNNN: PC=V0+NNN\n"
          "\t 0xCXNN: VX=rand()&NN\n"
          "\t 0xFX07: VX = dTimer\n"
          "\t 0xFX15: dTimer = VX\n"
          "\t 0xFX18: sTimer = VX\n"
          "\t 0xFX29E: I = sprite_addr[VX]");
    *lVal = rVal;
}

void skipNextIns(uint8_t cond)
{
    logOp("0x3XNN: Skip next instruction if VX==NN\n"
          "\t 0x4XNN: Skip next instruction if VX!=NN\n"
          "\t 0x5XY0: Skip next instruction if VX==VY\n"
          "\t 0x9XYN: Skip next instruction if VX!=VY\n"
          "\t 0xEX93: Skip next instruction if key()==VX\n"
          "\t 0xEXA1: Skip next instruction if key()!=VX");
    if(cond)
        cpu.PC += 2;
}

void addNoCarry(uintptr_t *lVal, const uint16_t rVal)
{
    logOp("0x7XNN: VX += NN (No Carry)\n"
          "\t 0xFX1E: I += VX (No Carry)");
    *lVal += rVal;
}

void addVXWithOverflow(uint8_t rVal, const uint8_t cond)
{
    logOp("0x8XY4: VX += VY (With VF Carry)\n"
          "\t 0x8XY5: VX -= VY (With VF Borrow)\n"
          "\t 0x8XY7: VX=VY-VX (with VF carry)");
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
