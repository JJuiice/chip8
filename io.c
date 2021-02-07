#include "io.h"
#include "logging.h"
#include <stdio.h>

static Display display;

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

void setupIO(const char *name, const short winNameLen) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        logQuit("Unable to initialize SDL");

    char winName[winNameLen + 1];
    memcpy(winName, name, winNameLen);
    winName[winNameLen] = 0;

    display.window = SDL_CreateWindow(
                    winName,
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    GFX_WIDTH * SCREEN_SCALE, GFX_HEIGHT * SCREEN_SCALE,
                    SDL_WINDOW_SHOWN
                    );
    checkSDLError(__LINE__, "Error creating window");

    display.renderer = SDL_CreateRenderer(
                    display.window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                    );
    checkSDLError(__LINE__, "Error creating renderer");
    
    display.texture = SDL_CreateTexture(
                    display.renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STATIC,
                    GFX_WIDTH,
                    GFX_HEIGHT
                    );
    checkSDLError(__LINE__, "Error creating renderer");
}

void drawGfx() {
    #ifndef NDEBUG
    logMsg("Drawing Graphics\n");
    #endif

    SDL_UpdateTexture(display.texture, NULL, gfx, GFX_WIDTH * sizeof(uint32_t));
    checkSDLError(__LINE__, "Error updating texture");

    SDL_RenderCopy(display.renderer, display.texture, NULL, NULL);
    checkSDLError(__LINE__, "Error copying texture to renderer");

    SDL_RenderPresent(display.renderer);
    checkSDLError(__LINE__, "Error copying texture to renderer");
}

void cleanGfx() {
    SDL_DestroyTexture(display.texture);
    checkSDLError(__LINE__, "Error destroying texture");
    
    SDL_DestroyRenderer(display.renderer);
    checkSDLError(__LINE__, "Error destroying renderer");

    SDL_DestroyWindow(display.window);
    checkSDLError(__LINE__, "Error destroying window");
    
    SDL_Quit();
}
