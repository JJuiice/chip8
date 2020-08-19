#ifndef CHIP8
#define CHIP8

void init(void),
     loadGame(const char *),
     emulateCycle(void),
     setKeys(void);

#endif
