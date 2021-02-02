#ifndef CHIP8_H
#define CHIP8_H

extern unsigned int drawFlag;
extern unsigned char gfx[GFX_RESOULTION];
extern const unsigned char fontset[80];

void init(void);
void loadGame(const char*);
void emulateCycle(void);

#endif
