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
    char *filename;
    int exec;
    uint32_t sdlTimestamp;
} Context;

const uint16_t FRAME60_MS = 1000 / 60;
const uint16_t CLOCK_MS = 1000 / CLOCK_SPEED;
 
static void getGameName(Context *ctx)
{
    if(strlen(ctx->filepath) < 5) {
        ctx->exec = !logMsgQuit("A VALID *.ch8 BINARY FILE MUST BE PROVIDED");
        return;
    }
 
    char delimiter;
#if defined _WIN32 || defined __CYGWIN__
    delimiter = '\\';
#else
    delimiter = '/';
#endif

    char *filename = NULL;
    char *nameDelim = strrchr(ctx->filepath, delimiter);
    if(nameDelim)
        filename = nameDelim + 1;
    else
        filename = ctx->filepath; 
    
    ctx->filename = strdup(filename);
}

static void init(Context *ctx)
{
    getGameName(ctx);
    gameInit();
    loadGame(ctx->filepath);

    if(!(ctx->exec))
        return;

    ctx->exec = !setupIO(ctx->filename);
    free(ctx->filename);
    ctx->filename = NULL;
}

static void gameLoop(void *arg)
{
    Context *ctx = arg;
    uint32_t startTick = getSDLTimestamp();
        
    if(startTick - ctx->sdlTimestamp >= FRAME60_MS) {
        ctx->exec = !updateTimers();
        ctx->sdlTimestamp = getSDLTimestamp();
    }

    ctx->exec = ctx->exec ? !emulateCycle() : ctx->exec;

    if(ctx->exec && cpu.dFlag) {
        ctx->exec = !drawGfx();
        cpu.dFlag = 0;
    }

    if(ctx->exec) {
        uint32_t emulationSpeed = getSDLTimestamp() - startTick;
        if (emulationSpeed < CLOCK_MS)
            delayGfx(CLOCK_MS - emulationSpeed);
           
        ctx->exec = !recvEvtQuit();
    }
 
#ifdef __EMSCRIPTEN__
    if(!(ctx->exec))
        emscripten_cancel_main_loop();
#endif
}

static void cleanup(Context *ctx)
{
    logMsg("Cleaning IO");
    ctx->exec = cleanIO();
    closeLogFile();
}

int main(int argc, char **argv)
{
    Context ctx;
    ctx.exec = 1;

    openLogFile();

    if(argc != 2) {
        ctx.exec = logMsgQuit("SINGLE CHIP-8 BINARY ARGUMENT REQUIRED");
        closeLogFile();
        return ctx.exec;
    }

    ctx.filepath = argv[1];

    init(&ctx);

    if(ctx.exec) {
        ctx.sdlTimestamp = getSDLTimestamp();
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(gameLoop, &ctx, 0, EMS_SIM_INF_LOOP);
#else
        while(ctx.exec)
            gameLoop(&ctx);
#endif
    }
    
    cleanup(&ctx);
    return ctx.exec;
}
