#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <math.h>
#define SPEC_FREQ 44100
#define SAMPLE_SIZE 1024 

static Display display;
static double tVal;

static const uint8_t DOUBLE_PI = M_PI * 2;
static const double WAVE_FREQ = 900;

SDL_AudioDeviceID sound;
uint32_t gfx[GFX_RESOULTION];
const uint8_t fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

static void soundCallback(void *udata, uint8_t *stream, int len)
{
    int streamLen = len > SAMPLE_SIZE ? SAMPLE_SIZE : len / sizeof(*stream);

    for(int i = 0; i < streamLen; i++) {
        stream[i] = 32 * sin(tVal); 
        tVal += 1.0f * (WAVE_FREQ * DOUBLE_PI) / SPEC_FREQ;
        tVal = tVal >= DOUBLE_PI ? tVal - DOUBLE_PI : tVal;
    }

}

void setupIO(const char *name, const short winNameLen) {
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    checkSDLError(__LINE__);

    char winName[winNameLen + 1];
    memcpy(winName, name, winNameLen);
    winName[winNameLen] = 0;

    tVal = 0;

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = SPEC_FREQ;
    spec.format = AUDIO_U8;
    spec.channels = 1;
    spec.samples = SAMPLE_SIZE;
    spec.callback = soundCallback;

    sound = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    checkSDLError(__LINE__);

    display.window = SDL_CreateWindow(
                    winName,
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    GFX_WIDTH * SCREEN_SCALE, GFX_HEIGHT * SCREEN_SCALE,
                    SDL_WINDOW_SHOWN
                    );
    checkSDLError(__LINE__);

    display.renderer = SDL_CreateRenderer(
                    display.window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                    );
    checkSDLError(__LINE__);
    
    display.texture = SDL_CreateTexture(
                    display.renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STATIC,
                    GFX_WIDTH,
                    GFX_HEIGHT
                    );
    checkSDLError(__LINE__);
}

void drawGfx() {
    #ifndef NDEBUG
    logMsg("Drawing Graphics\n");
    #endif

    SDL_UpdateTexture(display.texture, NULL, gfx, GFX_WIDTH * sizeof(uint32_t));
    checkSDLError(__LINE__);

    SDL_RenderCopy(display.renderer, display.texture, NULL, NULL);
    checkSDLError(__LINE__);

    SDL_RenderPresent(display.renderer);
    checkSDLError(__LINE__);
}

void cleanIO() {
    SDL_DestroyTexture(display.texture);
    checkSDLError(__LINE__);
    
    SDL_DestroyRenderer(display.renderer);
    checkSDLError(__LINE__);

    SDL_DestroyWindow(display.window);
    checkSDLError(__LINE__);

    SDL_CloseAudioDevice(sound);
    checkSDLError(__LINE__);
    
    SDL_Quit();
}
