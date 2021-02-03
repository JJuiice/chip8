#include "ins.h"
#include "logging.h"
#include <stdio.h>

static void logOp(const char *msg, uint16_t ins)
{
    #ifndef NDEBUG
    char errMsg[2 + sizeof(ins) + 3 + strlen(msg) + 2];
    sprintf(errMsg, "0x%X - %s\n", ins, msg);
    logErr(msg);
    #endif
}

void jmp(CPU *cpu)
{  
    logOp("0x1NNN: JMP NNN", cpu->opcode.ins);
    cpu->PC = cpu->opcode.addr;
    cpu->PC -= 2;
}

void call(CPU *cpu)
{
    logOp("0x2NNN: Call NNN", cpu->opcode.ins);
    cpu->stack[cpu->SP] = cpu->PC;
    ++cpu->SP;
    cpu->PC = cpu->opcode.addr;
    cpu->PC -= 2;
}

void setValUInt8(uint16_t ins, uint8_t *lVal, uint8_t rVal)
{
    logOp("0x6XNN: VX = NN\n \
           0xFX07: VX = dTimer\n \
           0x8XY0: VX = VY\n \
           0x8XY1: VX = VX | VY\n \
           0x8XY2: VX = VX & VY\n \
           0x8XY3: VX = VX ^ VY\n \
           0xFX15: dTimer = VX\n \
           0xFX18: sTimer = VX", ins);
    *lVal = rVal;
}

void setValUInt16(uint16_t ins, uint16_t *lVal, uint16_t rVal)
{
    logOp("0xANNN: I = NNN\n \
           0xFX29E: I = sprite_addr[VX]", ins);
   *lVal = rVal;
}

void skipNextIns(CPU *cpu, uint8_t skip)
{
    logOp("0xZXNN: Skip next instruction if {Z==3: VX==NN, Z==4: VX!=NN, Z==5: VX==VY, Z==9: VX!=VY}\n \
           0xEXVB: Skip next instruction if {VB=9E: key()==VX, VB==A!: key()!=VX}", cpu->opcode.ins);
    if(skip)
        cpu->PC += 2;
}

void addNoCarry(CPU *cpu)
{
    logOp("0x7XNN: VX += NN (No Carry)", cpu->opcode.ins);
    cpu->V[cpu->opcode.x] += cpu->opcode.nn;
}

void addWithCarry(CPU *cpu, uint8_t rVal)
{
    logOp("0x8XY4: VX += VY\n \
           0x8XY5: VX -= VY (All with VF Carry)", cpu->opcode.ins);
}

void dispClear(uint16_t ins, uint8_t *dFlag)
{
    logOp("0x00E0: Clear display", ins);
    memset(gfx, 0, sizeof(gfx));
    *dFlag = 1;
}

void ret(CPU *cpu)
{
    logOp("0x00EE: RET", cpu->opcode.ins);
    --cpu->SP;
    cpu->PC = cpu->stack[cpu->SP];
    cpu->PC -= 2;
}
