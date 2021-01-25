#ifndef CHIP8
#define CHIP8
#define GFX_WIDTH 64
#define GFX_HEIGHT 32

extern unsigned int drawFlag;

void emulateCycle(void),
     setKeys(void);

int loadGame(const char *);
int init(void);

#endif
