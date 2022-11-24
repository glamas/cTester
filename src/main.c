
#include <stdio.h>
#include "gui_sdl.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;


int main(int args, char *argv[])
{
    SDL_Window *win = SDL_CreateWindow("cTester", 100, 100, SCREEN_WIDTH,
        SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    GuiSDL_Init(win);
    // layout

    while ( ! UserData.quit) {
        while (SDL_PollEvent(&UserData.event)){
            switch(UserData.event.type)
            {
                case SDL_QUIT:
                    UserData.quit = 1;
                    break;
            }
        }
        // draw
        GuiSDL_Draw();

        // delay
        GuiSDL_Delay();
    }
    GuiSDL_Free();

    return 0;
}