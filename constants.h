#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef LNX
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>

#define GFX_WIDTH 64
#define GFX_HEIGHT 32
#define GFX_RESOULTION (GFX_WIDTH * GFX_HEIGHT)
#define KEY_NUM 16

extern const SDL_Scancode key_map[KEY_NUM];

#endif