#ifndef CHIP8
#define CHIP8
#include <SDL2/SDL.h>

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_RESOULTION (GFX_WIDTH * GFX_HEIGHT)
#define KEY_NUM 16

extern unsigned int drawFlag;
extern unsigned char gfx[GFX_RESOULTION];
extern unsigned char fontset[80];

void emulateCycle(void),
     setKeys(void);

int loadGame(const char *);
int init(void);

#endif
