#ifndef CHIP8
#define CHIP8

#define TRUE 1
#define FALSE 0

void init(void),
     loadGame(const char *),
     emulateCycle(void),
     setKeys(void);

#endif
