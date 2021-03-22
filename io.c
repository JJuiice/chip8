/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define PIXEL_ON 0xFFFFFF
#define SCREEN_SCALE 10 
#define GFX_RESOULTION (GFX_WIDTH * GFX_HEIGHT)

#define SPEC_FREQ 44100
#define SAMPLE_SIZE 1024 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

static SDL_Event event;
static Display display;
static SDL_AudioDeviceID sound;
static uint32_t gfx[GFX_RESOULTION];
static double tVal;

static const uint8_t DOUBLE_PI = M_PI * 2;
static const double WAVE_FREQ = 900;

static const SDL_Scancode key_map[KEY_NUM] =
{
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
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

static int checkSDLErr(int line)
{
    int isErr = 0;
	const char *error = SDL_GetError();

	if (*error != '\0')
	{
        char sdlError[100];
        sprintf(sdlError, "SDL Error: %s\n + line: %i", error, line);

        isErr = logMsgQuit(sdlError);

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", sdlError);
        SDL_ClearError();
	}

    return isErr;
}

int setupIO(const char *name)
{
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO))
        return checkSDLErr(__LINE__);

   
    tVal = 0;

    SDL_AudioSpec spec;
    SDL_zero(spec);
    spec.freq = SPEC_FREQ;
    spec.format = AUDIO_U8;
    spec.channels = 1;
    spec.samples = SAMPLE_SIZE;
    spec.callback = soundCallback;

    sound = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

    if(!sound)
        return checkSDLErr(__LINE__);

    display.window = SDL_CreateWindow(
                    name,
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    GFX_WIDTH * SCREEN_SCALE, GFX_HEIGHT * SCREEN_SCALE,
                    SDL_WINDOW_SHOWN
                    );
    if(!display.window)
        return checkSDLErr(__LINE__);

    display.renderer = SDL_CreateRenderer(
                    display.window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                    );
    if(!display.renderer)
        return checkSDLErr(__LINE__);
    
    display.texture = SDL_CreateTexture(
                    display.renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STATIC,
                    GFX_WIDTH,
                    GFX_HEIGHT
                    );
    if(!display.texture)
        return checkSDLErr(__LINE__);

#ifdef __EMSCRIPTEN__
    SDL_ClearError();
#endif
    return 0;
}

int cleanIO(void) {
    SDL_DestroyTexture(display.texture);
    if(checkSDLErr(__LINE__))
        return -1;
    
    SDL_DestroyRenderer(display.renderer);
    if(checkSDLErr(__LINE__))
        return -1;

    SDL_DestroyWindow(display.window);
    if(checkSDLErr(__LINE__))
        return -1;

    SDL_CloseAudioDevice(sound);
    if(checkSDLErr(__LINE__))
        return -1;
    
    SDL_Quit();
    return 0;
}

int drawGfx(void)
{
#ifndef NDEBUG
    logMsg("Drawing Graphics\n");
#endif

    if(SDL_UpdateTexture(display.texture, NULL, gfx, GFX_WIDTH * sizeof(uint32_t)))
        return checkSDLErr(__LINE__);

    if(SDL_RenderCopy(display.renderer, display.texture, NULL, NULL))
        return checkSDLErr(__LINE__);

    SDL_RenderPresent(display.renderer);
    
    return checkSDLErr(__LINE__);
}

void clrGfx(void)
{
    memset(gfx, ~PIXEL_ON, sizeof(gfx));
}

void flipPx(uint32_t gfxInx)
{
    gfx[gfxInx] ^= PIXEL_ON;
}

void delayGfx(uint32_t ms)
{
    SDL_Delay(ms);
}

int isPxOn(uint32_t gfxInx)
{
    return gfx[gfxInx] == PIXEL_ON;
}

int isAudioPaused(void)
{
    return (SDL_GetAudioDeviceStatus(sound) != SDL_AUDIO_PLAYING);
}

int pauseAudio(uint8_t status, int line)
{
    SDL_PauseAudioDevice(sound, status);
    return checkSDLErr(line);
}

const uint8_t* getKeyboardState(void)
{
    return SDL_GetKeyboardState(NULL);
}

uint8_t isKeyPressed(const uint8_t *keyboardState, int keyInx)
{
    return keyboardState[key_map[keyInx]];
}

int getSDLTimestamp(void)
{
    return SDL_GetTicks();
}

int recvEvtQuit(void)
{
    int recv = 0;

    while(SDL_PollEvent(&event))
        if(event.type == SDL_QUIT)
            recv = 1;

    return recv;
}
