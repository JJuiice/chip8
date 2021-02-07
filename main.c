#include "constants.h"
#include "cpu.h"
#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>

#define CLOCK_SPEED 500 

#define MAX_FILE_LEN 255
#define C8_DELIM_LEN_OFFSET -4 

static void getGameName(char name[MAX_FILE_LEN])
{
    printf("Enter name: ");
    fgets(name, MAX_FILE_LEN, stdin);

    const short nameLength = strlen(name);
    if (nameLength <= 0 || nameLength > MAX_FILE_LEN ) 
        logQuit("Invalid file name");
    
    name[strlen(name) - 1] = 0;
}

int main(int argc, char **argv)
{
    const uint16_t FRAME60_MS = 1000 / 60;
    const uint16_t CLOCK_MS = 1000 / CLOCK_SPEED;
    char name[MAX_FILE_LEN];

    #ifndef NDEBUG
    const char *fName = "Airplane.ch8"; // "test_opcode.ch8"; 
    sprintf(name, "%s", fName);
    #else
    getGameName(name);
    #endif
    const short winNameLen = strlen(name) + C8_DELIM_LEN_OFFSET;

    setupIO(name, winNameLen);

    init();

    loadGame(name);

    SDL_Event event;
    uint32_t timerTick = SDL_GetTicks();
    uint8_t exec = 1;
    while(exec) {
        uint32_t startTick = SDL_GetTicks();
        
        if(startTick - timerTick >= FRAME60_MS) {
            updateTimers();
            timerTick = SDL_GetTicks();
        }

        emulateCycle();
        if(cpu.dFlag) {
            drawGfx();
            cpu.dFlag = 0;
        }
        
        uint32_t emulationSpeed = SDL_GetTicks() - startTick;
        if (emulationSpeed < CLOCK_MS)
            SDL_Delay(CLOCK_MS - emulationSpeed);


        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT)
                exec = 0;
        }
   }

    cleanIO();
    exit(0);
}
