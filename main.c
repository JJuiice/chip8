#include "chip8.h"

unsigned short stack[16],
               op,
               I,
               PC,
               SP;

unsigned char mem[4096],
              V[16],
              gfx[64 *32],
              cKey[16],
              dTimer,
              sTimer;

short dFlag;

int main(int argc, char **argv)
{
    while(TRUE) {

    }

    return 0;
}
