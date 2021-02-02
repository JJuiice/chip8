#ifndef CHIP8_H
#define CHIP8_H

#define PIXEL_ON 0xFFFFFFFF
#define PIXEL_OFF 0xFF000000

extern uint8_t drawFlag;
extern uint32_t gfx[GFX_RESOULTION];
extern const uint8_t fontset[80];

void init(void);
void loadGame(const char*);
void emulateCycle(void);

#endif
