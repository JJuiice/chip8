#ifndef GFX_H
#define GFX_H

#if defined _WIN32 || defined __CYGWIN__
#define SDL_MAIN_HANDLED
#endif

#include "constants.h"
#include <SDL2/SDL.h>

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_RESOULTION (GFX_WIDTH * GFX_HEIGHT)
#define SCREEN_SCALE 10 
#define PIXEL_ON 0xFFFFFF

typedef struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

void setupIO(const char *name);
void drawGfx();
void cleanIO();

extern SDL_AudioDeviceID sound;
extern uint32_t gfx[GFX_RESOULTION];
extern const uint8_t fontset[80];
extern const SDL_Scancode key_map[KEY_NUM];
#endif
