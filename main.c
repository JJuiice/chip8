#include "constants.h"
#include "cpu.h"
#include "gfx.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>

#define FPS 60
#define MS_PER_FPS (1000 / FPS)

#define MAX_FILE_LEN 256

static void askGameName(char name[MAX_FILE_LEN])
{
    printf("Enter name: ");
    fgets(name, MAX_FILE_LEN, stdin);
}

int main(int argc, char **argv)
{
    Display display;
    CPU cpu;
    char name[MAX_FILE_LEN];

    setupGfx(&display, "Chip8");

    init(&cpu);

    askGameName(name);
    name[strlen(name) - 1] = '\0';

    loadGame(&cpu, name);

    SDL_Event event;
    uint8_t run = 1;
    while(run) {
        uint32_t startTick = SDL_GetTicks();

        emulateCycle(&cpu);
        if(cpu.dFlag) {
            drawGfx(&display);
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

    cleanGfx(&display);
    exit(0);
}
