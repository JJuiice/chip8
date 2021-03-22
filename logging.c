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
#include <stdio.h>
#include <stdlib.h>

static FILE *logFile = NULL;

void openLogFile(void)
{
#ifndef __EMSCRIPTEN__
    logFile = fopen("output.log", "w");
#else
    logFile = stderr;
#endif
}

void closeLogFile(void)
{
#ifndef __EMSCRIPTEN__
    fclose(logFile);
#endif
}

void logMsg(const char *msg)
{
    fprintf(logFile, "%s\n", msg);
}

int logMsgQuit(const char *msg)
{
    logMsg(msg);
    return -1;
}

void dumpRegAndPointerInfo(void)
{
    fprintf(logFile, "\nPC: 0x%X\nI: 0x%X\nSTACK: ", cpu.PC, cpu.I);
    for(int i = 0; i < cpu.SP; i++)
        fprintf(logFile, "0x%X", cpu.stack[i]);

    fprintf(logFile, "\nDT: %d\nST: %d\n", cpu.dTimer, cpu.sTimer);

    for(int i = 0; i < 16; i++)
        fprintf(logFile, "V[0x%X] = 0x%X (%d)\n", i, cpu.V[i], cpu.V[i]);
    fprintf(logFile, "\n");
}
