
#include <stdio.h>
#include "gui_sdl.h"
#include "gui_sdl_ui.h"

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;


int main(int args, char *argv[])
{
    SDL_Window *win = SDL_CreateWindow("cTester", 100, 100, SCREEN_WIDTH,
        SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    gui_sdl_init(win);
    // layout
    DebugInfo("debug_1", 0, 0);

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
        gui_sdl_draw();

        // delay
        gui_sdl_delay();
    }
    gui_sdl_free();

    return 0;
}