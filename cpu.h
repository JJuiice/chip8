#ifndef CPU_H
#define CPU_H
#include <stdint.h>

#define PIXEL_ON 0xFFFFFFFF
#define PIXEL_OFF 0xFF000000

#define MEM_SIZE 4096
#define STACK_SIZE 16

union Opcode
{
    uint16_t ins;
    struct
    {
        uint8_t n  : 4;
        uint8_t x  : 4;
        uint8_t y  : 4;
        uint8_t op : 4;
    };
    struct
    {
        uint8_t nn : 8;
    };
    struct
    {
        uint16_t addr : 12;
    };
};

typedef struct CPU
{
    union Opcode opcode;

    uint16_t I;
    uint16_t PC;
    uint16_t SP;
    uint16_t stack[STACK_SIZE];

    uint8_t mem[MEM_SIZE];
    uint8_t V[16];

    uint8_t dTimer;
    uint8_t sTimer;

    uint8_t dFlag;
} CPU;

void init(CPU *);
void loadGame(CPU *, const char*);
void emulateCycle(CPU *);

#endif
