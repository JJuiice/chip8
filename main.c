#include "main.h"
#include "chip8.h"

#define SCREEN_SCALE 10
#define SCREEN_BPP 32

int main(int argc, char **argv)
{
    if(argc <= 1) {
        fprintf(stderr, "No game file specified!");
    }

    if(setupGfx(argv[1]) || setupInput() || init() || loadGame(argv[1]))
        return 1;

    while(RUN_EMULATION) {
        emulateCycle();

        if(drawFlag)
            drawGfx();
        
        setKeys();
    }

    return 0;
}

void sdlQuit(const char *msg)
{
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}

void checkSDLError(int line)
{
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
		fprintf(stderr, "SDL Error: %s\n", error);
		if (line != -1)
			fprintf(stderr, " + line: %i\n", line);
		SDL_ClearError();
	}
#endif
}

int setupGfx(const char *game) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        sdlQuit("Unable to initialize SDL");

    SDL_Event event;
    SDL_SetVideoMode(GFX_WIDTH*SCREEN_SCALE, GFX_HEIGHT*SCREEN_SCALE, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

    checkSDLError(__LINE__);

    return 0;
}

int setupInput(void) {

}

int drawGfx(void) {

}
