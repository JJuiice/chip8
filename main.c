#include "chip8.h"
#include <string.h>

#define SCREEN_SCALE 10
#define SCREEN_BPP 32

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

int main(int argc, char **argv)
{
    KeyMap keyMap[KEY_NUM];
    Display display;

    if(argc <= 1) {
        fprintf(stderr, "No game file specified!");
    }

    if(setupGfx(&display, argv[1]) || setupInput(&keyMap) || init() || loadGame(argv[1]))
        return 1;

    int run = 1;
    while(run) {
        emulateCycle();

        if(drawFlag)
            drawGfx(&display);
        
        setKeys();
    }

    cleanGfx(&display);
    return 0;
}

int sdlErr(const char *msg)
{
    printf("%s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    return 1;
}

int checkSDLError(int line)
{
	const char *error = SDL_GetError();
	if (*error != '\0')
	{
        char sdlError[500];
        sprintf(sdlError, "SDL Error: %s\n", *error);
		if (line != -1)
			sprintf(*sdlError, "%s + line: %i\n", *sdlError, line);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, *sdlError);
		SDL_ClearError();
        #ifndef NDEBUG
        fprintf(stderr, *sdlError);
        #endif
        return 1;
	}
    return 0;
}

int setupGfx(Display *display, const char *game) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return sdlErr("Unable to initialize SDL");

    display->window = SDL_CreateWindow(
                    *game,
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    GFX_WIDTH * SCREEN_SCALE, GFX_HEIGHT * SCREEN_SCALE,
                    SDL_WINDOW_SHOWN
                    );

    if (checkSDLError(__LINE__))
        return sdlErr("Error creating window");

    display->renderer = SDL_CreateRenderer(
                    display->window,
                    -1,
                    SDL_RENDERER_ACCELERATED
                    );
    
    if (checkSDLError(__LINE__))
        return sdlErr("Error creating renderer");
    
    display->texture = SDL_CreateTexture(
                    display->renderer,
                    SDL_PIXELFORMAT_ABGR8888,
                    SDL_TEXTUREACCESS_STATIC,
                    GFX_WIDTH,
                    GFX_HEIGHT
                    );

    if (checkSDLError(__LINE__))
        return sdlErr("Error creating renderer");
 
    return 0;
}

int drawGfx(Display *display) {
    SDL_UpdateTexture(display->texture, NULL, gfx, GFX_WIDTH * sizeof(char));

    if (checkSDLError(__LINE__))
        return sdlErr("Error updating texture");

    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);

    if (checkSDLError(__LINE__))
        return sdlErr("Error copying texture to renderer");

    SDL_RenderPresent(display->renderer);

    if (checkSDLError(__LINE__))
        return sdlErr("Error copying texture to renderer");
    
    return 0;
}

int cleanGfx(Display *display) {
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
}

int setupInput(KeyMap keyMap[KEY_NUM]) {
    int error = 0;
    const short valueLength = 5;
    const SDL_Scancode *keys = {
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

    if(checkSDLError(__LINE__))
        error = sdlErr("Error mapping keyboard");

    return error;
}
