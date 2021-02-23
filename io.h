/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#ifndef GFX_H
#define GFX_H

#if defined _WIN32 || defined __CYGWIN__
#define SDL_MAIN_HANDLED
#endif

#include "constants.h"
#include <stdint.h>

#define GFX_WIDTH 64
#define GFX_HEIGHT 32

void setupIO(const char *name);
void cleanIO(void);

void drawGfx(void);
void clrGfx(void);
void flipPx(uint32_t gfxInx);
void delayGfx(uint32_t ms);
int isPxOn(uint32_t gfxInx);

int isAudioPaused(void);
void pauseAudio(uint8_t status);

int getSDLTimestamp(void);
int recvEvtQuit(void);

const uint8_t* getKeyboardState(void);
uint8_t isKeyPressed(const uint8_t *keyboardState, int keyInx);

void closeSDLErr(const char *msg);
void checkSDLErr(int line);

#endif
