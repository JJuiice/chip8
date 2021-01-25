#include "main.h"
#include "chip8.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main(int argc, char **argv)
{
    if(setupGfx() || setupInput() || init() || loadGame("pong"))
        return 1;

    while(RUN_EMULATION) {
        emulateCycle();

        if(drawFlag)
            drawGfx();
        
        setKeys();
    }

    return 0;
}

int setupGfx(void) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(GFX_WIDTH, GFX_HEIGHT, "Chip8", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    return 0;
}

void setupInput(void) {

}

void drawGfx(void) {

}
