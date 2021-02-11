#include "constants.h"
#include "cpu.h"
#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>

#define CLOCK_SPEED 500 

#define C8_DELIM_LEN_OFFSET -4 

static void getGameName(char *filepath, char **gameName)
{
    if(strlen(filepath) < 5)
        logErrQuit("A valid .ch8 binary file must be provided");
 
    char delimiter;
    #if defined _WIN32 || defined __CYGWIN__
        delimiter = '\\';
    #else
        delimiter = '/';
    #endif

    char *filename = NULL;
    char *nameDelim = strrchr(filepath, delimiter);
    if(nameDelim)
        filename = nameDelim + 1;
    else
        filename = filepath; 
    
    const short filenameLen = strlen(filename) + C8_DELIM_LEN_OFFSET;
    *gameName = (char *) malloc(filenameLen + 1);
    memcpy(*gameName, filename, filenameLen);
    (*gameName)[filenameLen] = 0;
}

int main(int argc, char **argv)
{
    const uint16_t FRAME60_MS = 1000 / 60;
    const uint16_t CLOCK_MS = 1000 / CLOCK_SPEED;
    char *name;

    logFile = fopen("output.log", "w");

    if(argc != 2)
        logErrQuit("Single Chip-8 binary argument required");

    getGameName(argv[1], &name);

    setupIO(name);
    free(name);
    name = NULL;

    init();

    loadGame(argv[1]);

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
    fclose(logFile);
    exit(0);
}
