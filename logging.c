/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#include "constants.h"
#include "logging.h"
#include "cpu.h"
#include "io.h"

#define QUIT_DELAY_MS 2000

FILE *logFile = NULL;

void logMsg(const char *msg)
{
    fprintf(logFile, "%s\n", msg);
}

void logSDLErrQuit(const char *msg)
{
    logMsg(msg);
    SDL_ClearError();
    SDL_Delay(QUIT_DELAY_MS);
    SDL_Quit();
    fclose(logFile);
    exit(-1);
}

void logErrQuit(const char *msg)
{
    logMsg(msg);
    fclose(logFile);
    exit(-1);
}

void checkSDLError(int line)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
        char sdlError[100];
        sprintf(sdlError, "SDL Error: %s\n + line: %i", error, line);
        
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s\n", sdlError);
        logSDLErrQuit(sdlError);
	}
}

void dumpRegAndPointerInfo()
{
    fprintf(logFile, "\nPC: 0x%X\nI: 0x%X\nStack: ", cpu.PC, cpu.I);
    for(int i = 0; i < cpu.SP; i++)
        fprintf(logFile, "0x%X", cpu.stack[i]);

    fprintf(logFile, "\nDelay Timer: %d\nSound Timer: %d\n", cpu.dTimer, cpu.sTimer);

    for(int i = 0; i < 16; i++)
        fprintf(logFile, "V[0x%X] = 0x%X (%d)\n", i, cpu.V[i], cpu.V[i]);
    fprintf(logFile, "\n");
}
