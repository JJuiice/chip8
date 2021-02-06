#include <stdio.h>
#include "constants.h"
#include "logging.h"
#include "cpu.h"

#define QUIT_DELAY_MS 3000

void logErr(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

void logQuit(const char *msg)
{
    logErr(msg);
    SDL_ClearError();
    SDL_Delay(QUIT_DELAY_MS);
    SDL_Quit();
    exit(1);
}

void checkSDLError(int line, const char *msg)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
        char *fullError;
        char *sdlError;
        snprintf(sdlError, 13 + strlen(error),"SDL Error: %s\n", error);
		if (line != -1)
			snprintf(fullError, strlen(sdlError) + 11 + sizeof(int), "%s + line: %i\n", sdlError, line);
        else
            fullError = sdlError;
        
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", fullError);
        logQuit(msg);
	}
}

void dumpRegAndPointerInfo()
{
    fprintf(stderr, "\nPC: 0x%X\nI: 0x%X\nStack: ", cpu.PC, cpu.I);
    for(int i = 0; i < cpu.SP; i++)
        fprintf(stderr, "0x%X", cpu.stack[i]);

    fprintf(stderr, "\nDelay Timer: %d\nSound Timer: %d\n", cpu.dTimer, cpu.sTimer);

    for(int i = 0; i < 16; i++)
        fprintf(stderr, "V[0x%X] = 0x%X (%d)\n", i, cpu.V[i], cpu.V[i]);
    fprintf(stderr, "\n");
}
