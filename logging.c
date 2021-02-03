#include <stdio.h>
#include "constants.h"
#include "logging.h"

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
