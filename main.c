/*
 * Copyright (C) 2020-2021 Ojas Anand
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
 */

#include "constants.h"
#include "cpu.h"
#include "io.h"
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __EMSCRIPTEN__
#define EMS_SIM_INF_LOOP 1
#include <emscripten.h>
#endif

#define CLOCK_SPEED 500 

#define C8_DELIM_LEN_OFFSET -4 

typedef struct Context {
    char *filepath;
} Context;

static void getGameName(char *filepath, char **gameName)
{
    if(strlen(filepath) < 5)
        logErrQuit("A VALID *.ch8 BINARY FILE MUST BE PROVIDED");
 
    char delimiter;
    #if defined _WIN32 || defined __CYGWIN__
        delimiter = '\\';
    #else
        delimiter = '/';
    #endif

    char *filename = NULL;
    char *nameDelim = strrchr(filepath, delimiter);
    if(nameDelim)
        filename = nameDelim + 1;
    else
        filename = filepath; 
    
    const short filenameLen = strlen(filename) + C8_DELIM_LEN_OFFSET;
    *gameName = (char *) malloc(filenameLen + 1);
    memcpy(*gameName, filename, filenameLen);
    (*gameName)[filenameLen] = 0;
}

void game(void *arg)
{
    const uint16_t FRAME60_MS = 1000 / 60;
    const uint16_t CLOCK_MS = 1000 / CLOCK_SPEED;

    Context *ctx = arg;
    char *name;

    getGameName(ctx->filepath, &name);

    setupIO(name);
    free(name);
    name = NULL;

    init();

    loadGame(ctx->filepath);

    uint32_t timerTick = getSDLTimestamp();
    uint8_t exec = 1;
    while(exec) {
        uint32_t startTick = getSDLTimestamp();
        
        if(startTick - timerTick >= FRAME60_MS) {
            updateTimers();
            timerTick = getSDLTimestamp();
        }

        emulateCycle();
        if(cpu.dFlag) {
            drawGfx();
            cpu.dFlag = 0;
        }
        
        uint32_t emulationSpeed = getSDLTimestamp() - startTick;
        if (emulationSpeed < CLOCK_MS)
            delayGfx(CLOCK_MS - emulationSpeed);

        exec = !recvEvtQuit();
  }

    cleanIO();
#ifdef __EMSCRIPTEN__
    emscripten_cancel_main_loop();
#endif
}

int main(int argc, char **argv)
{
    Context ctx;

    openLogFile();

    if(argc != 2)
        logErrQuit("SINGLE CHIP-8 BINARY ARGUMENT REQUIRED");

    ctx.filepath = argv[1];

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(game, &ctx, 0, EMS_SIM_INF_LOOP);
#else
    game(&ctx);
#endif

    closeLogFile();
    exit(0);
}
