#include "constants.h"
#include "cpu.h"
#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define CLOCK_SPEED 180 

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
    const double FRAME60_MS = 1000.0 / 60.0;
    const double CLOCK_MS = 1000.0 / CLOCK_SPEED;
    char name[MAX_FILE_LEN];

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1;
    spec.samples = 1024;
    spec.callback = NULL;


    SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    checkSDLError(__LINE__, "Error opening audio device");

    printf("COMPLETED!\n");
    float x = 0;
    for (int i = 0; i < spec.freq * 3; i++) {
        x += .010f;

        // SDL_QueueAudio expects a signed 16-bit value
        // note: "5000" here is just gain so that we will hear something
        int16_t sample = sin(x * 4) * 5000;

        const int sample_size = sizeof(int16_t) * 1;
        SDL_QueueAudio(device, &sample, sample_size);
    }

    // unpausing the audio device (starts playing):
    SDL_PauseAudioDevice(device, 0);
    SDL_Delay(3000);
    SDL_PauseAudioDevice(device, 1);
    #ifndef NDEBUG
    const char *fName = "Delay Timer Test [Matthew Mikolay, 2010].ch8"; // "test_opcode.ch8"; 
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

    cleanGfx();
    exit(0);
}
