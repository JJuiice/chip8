#ifndef GFX_H
#define GFX_H

#ifndef LNX
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_RESOULTION (GFX_WIDTH * GFX_HEIGHT)
#define SCREEN_SCALE 10 

typedef struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

void setupGfx(Display*, const char*);
void drawGfx(Display*);
void cleanGfx(Display*);

extern uint32_t gfx[GFX_RESOULTION];
extern const uint8_t fontset[80];
#endif
