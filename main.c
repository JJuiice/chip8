#include "chip8.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define SCREEN_SCALE 5 

#define FPS 60
#define MS_PER_FPS (1000 / FPS)

typedef struct KeyMap
{
    SDL_Scancode key;
    unsigned char *mapped_value;
} KeyMap;

typedef struct Display
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

void errQuit(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    SDL_ClearError();
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
		if (line != -1) {
            int numOfDigits = floor(log10(abs((double) line)) + 1);
			snprintf(fullError, strlen(sdlError) + 11 + numOfDigits, "%s + line: %i\n", sdlError, line);
        } else {
            fullError = sdlError;
        }
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", fullError);
        errQuit(msg);
	}
}

void setupGfx(Display *display, const char *game) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        errQuit("Unable to initialize SDL");

    display->window = SDL_CreateWindow(
                    game,
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    GFX_WIDTH * SCREEN_SCALE, GFX_HEIGHT * SCREEN_SCALE,
                    SDL_WINDOW_SHOWN
                    );
    checkSDLError(__LINE__, "Error creating window");

    display->renderer = SDL_CreateRenderer(
                    display->window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                    );
    checkSDLError(__LINE__, "Error creating renderer");
    
    display->texture = SDL_CreateTexture(
                    display->renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STATIC,
                    GFX_WIDTH,
                    GFX_HEIGHT
                    );
    checkSDLError(__LINE__, "Error creating renderer");
}

void drawGfx(Display *display) {
    SDL_UpdateTexture(display->texture, NULL, gfx, GFX_WIDTH * sizeof(char));
    checkSDLError(__LINE__, "Error updating texture");

    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    checkSDLError(__LINE__, "Error copying texture to renderer");

    SDL_RenderPresent(display->renderer);
    checkSDLError(__LINE__, "Error copying texture to renderer");
}

void cleanGfx(Display *display) {
    SDL_DestroyTexture(display->texture);
    checkSDLError(__LINE__, "Error destroying texture");
    
    SDL_DestroyRenderer(display->renderer);
    checkSDLError(__LINE__, "Error destroying renderer");

    SDL_DestroyWindow(display->window);
    checkSDLError(__LINE__, "Error destroying window");
    
    SDL_Quit();
}

void setupInput(KeyMap keyMap[KEY_NUM]) {
    int error = 0;
    const short valueLength = 5;
    const SDL_Scancode keys[] = {
        SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
        SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
        SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
        SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
    };

    int i;
    for(i = 0; i < KEY_NUM; i++) {
        keyMap[i].key = keys[i];
        memcpy(keyMap[i].mapped_value, &fontset[i*valueLength], valueLength);
    }

    checkSDLError(__LINE__, "Error mapping keyboard");
}

int main(int argc, char **argv)
{
    KeyMap keyMap[KEY_NUM];
    Display display;

    if(argc <= 1)
        errQuit("No game file specified!");

    setupGfx(&display, argv[1]);
    setupInput(keyMap);

    init();
    loadGame(argv[1]);

    SDL_Event event;
    int run = 1;
    while(run) {
        uint32_t startTick = SDL_GetTicks();

        emulateCycle();
        if(drawFlag) {
            drawGfx(&display);
            drawFlag = 0;
        }
        
        uint32_t emulationSpeed = SDL_GetTicks() - startTick;
        if (emulationSpeed < MS_PER_FPS)
            SDL_Delay(MS_PER_FPS - emulationSpeed);


        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                run = 0;
            }
        }
    }

    cleanGfx(&display);
    exit(0);
}
