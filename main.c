#include "constants.h"
#include "cpu.h"
#include "gfx.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>

#define FPS 60
#define MS_PER_FPS (1000 / FPS)

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
    char name[MAX_FILE_LEN];

    #ifndef NDEBUG
    const char *fName = "Delay Timer Test [Matthew Mikolay, 2010].ch8"; // "delay_timer_test.ch8"; // "test_opcode.ch8"; 
    sprintf(name, "%s", fName);
    #else
    getGameName(name);
    #endif
    const short winNameLen = strlen(name) + C8_DELIM_LEN_OFFSET;

    setupGfx(name, winNameLen);

    init();

    loadGame(name);

    SDL_Event event;
    uint8_t run = 1;
    while(run) {
        uint32_t startTick = SDL_GetTicks();

        emulateCycle();
        if(cpu.dFlag) {
            drawGfx();
            cpu.dFlag = 0;
        }
        
        uint32_t emulationSpeed = SDL_GetTicks() - startTick;
        if (emulationSpeed < MS_PER_FPS)
            SDL_Delay(MS_PER_FPS - emulationSpeed);


        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                run = 0;
            }
        }
    }

    cleanGfx();
    exit(0);
}
