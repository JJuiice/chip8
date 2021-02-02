#include "constants.h"
#include "chip8.h"
#include "error_management.h"
#include <math.h>
#include <stdio.h>

#define SCREEN_SCALE 5 

#define FPS 60
#define MS_PER_FPS (1000 / FPS)

#define MAX_FILE_LEN 256

typedef struct Display
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

static void setupGfx(Display *display, const char *game) {
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

static void drawGfx(Display *display) {
    SDL_UpdateTexture(display->texture, NULL, gfx, GFX_WIDTH * sizeof(char));
    checkSDLError(__LINE__, "Error updating texture");

    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    checkSDLError(__LINE__, "Error copying texture to renderer");

    SDL_RenderPresent(display->renderer);
    checkSDLError(__LINE__, "Error copying texture to renderer");
}

static void cleanGfx(Display *display) {
    SDL_DestroyTexture(display->texture);
    checkSDLError(__LINE__, "Error destroying texture");
    
    SDL_DestroyRenderer(display->renderer);
    checkSDLError(__LINE__, "Error destroying renderer");

    SDL_DestroyWindow(display->window);
    checkSDLError(__LINE__, "Error destroying window");
    
    SDL_Quit();
}

static void askGameName(char name[MAX_FILE_LEN])
{
    printf("Enter name: ");
    fgets(name, MAX_FILE_LEN, stdin);
}

int main(int argc, char **argv)
{
    Display display;
    char name[MAX_FILE_LEN];

    setupGfx(&display, argv[1]);

    init();
    askGameName(name);
    loadGame(name);

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
