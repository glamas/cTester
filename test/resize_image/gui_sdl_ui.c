
#include "gui_sdl.h"
#include "gui_sdl_widget.h"
#include "gui_sdl_ui.h"

extern TTF_Font *font;
extern WidgetNode *layout;
extern EventData UserData;

int button_theme(Widget *button){
    if (button->toUpdate == 0) {
        SDL_RenderCopy(UserData.renderer, button->texture, NULL, &button->ren_rect);
        return NOREFRESH;
    }
    printf("update button: %s\n", button->id);
    // free first
    SDL_FreeSurface(button->surface);
    if (button->texture)
        SDL_DestroyTexture(button->texture);
    // text surface
    SDL_Surface *textSurface = TTF_RenderUTF8_Shaded(font, button->text, button->fg, button->bg);
    // theme
    SDL_Rect borderRect = {.w=textSurface->w + 2, .h=textSurface->h + 2};
    SDL_Surface *buttonSurface = SDL_CreateRGBSurface(0, borderRect.w, borderRect.h, 32, 0, 0, 0, 0);
    SDL_SetSurfaceBlendMode(buttonSurface, SDL_BLENDMODE_BLEND);
    // set color
    Uint32 borderColor = SDL_MapRGBA(buttonSurface->format, 0xFF, 0x00, 0x00, 0xFF);
    SDL_FillRect(buttonSurface, NULL, SDL_MapRGBA(buttonSurface->format, button->bg.r, button->bg.g, button->bg.b, button->bg.a));
    // copy to buttonSurface
    borderRect.x = 1;
    borderRect.y = 1;
    SDL_BlitSurface(textSurface, NULL, buttonSurface, &borderRect);
    // add border 1 pixel
    SDL_LockSurface(buttonSurface);
    Uint32 *pixel = (Uint32 *)buttonSurface->pixels;
    int pixel_num = buttonSurface->w * buttonSurface->h - 1;
    for (int i=0; i < buttonSurface->w; i++){
        pixel[i] = borderColor;
        pixel[pixel_num - i] = borderColor;
    }
    for (int j=0; j < buttonSurface->h; j++) {
        pixel[buttonSurface->w * j] = borderColor;
        pixel[buttonSurface->w * (j + 1) - 1] = borderColor;
    }
    SDL_UnlockSurface(buttonSurface);
    button->surface = buttonSurface;
    button->ren_rect.w = button->surface->w;
    button->ren_rect.h = button->surface->h;
    button->texture = SDL_CreateTextureFromSurface(UserData.renderer, buttonSurface);
    SDL_FreeSurface(textSurface);
    SDL_RenderCopy(UserData.renderer, button->texture, NULL, &button->ren_rect);
    button->toUpdate = 0;
    return REFRESH;
}

Widget * Button(char *id, char *text, int x, int y){
    Widget *btn;
    btn = (Widget *)malloc(sizeof(Widget));
    gui_sdl_init_widget(btn);
    btn->id = id;
    btn->text = text;
    btn->ren_rect.x = x;
    btn->ren_rect.y = y;
    btn->updateSurface = &button_theme;
    gui_sdl_add_widget(btn);
    return btn;
}

int debug_theme(Widget *widget) {
    char info[256];
    char mouse_click[20];
    if (UserData.mouse.down) {
        sprintf(mouse_click, "%s %s",
            UserData.mouse.clicks == 2 ? "double" : "",
            UserData.mouse.button == SDL_BUTTON_LEFT ? "left" : \
            UserData.mouse.button == SDL_BUTTON_MIDDLE ? "middle" : \
            UserData.mouse.button == SDL_BUTTON_RIGHT ? "right": ""
        );
    } else {
        mouse_click[0] = '\0';
    }
    sprintf(info, "FPS:%-4d\nMouse:(%d, %d) %s\nInput:%c", UserData.fps, UserData.mouse.x, UserData.mouse.y, mouse_click, UserData.input);
    SDL_Surface *surface = TTF_RenderUTF8_Shaded_Wrapped(font, info, widget->fg, widget->bg, 0);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(UserData.renderer, surface);
    widget->ren_rect.w = surface->w;
    widget->ren_rect.h = surface->h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(UserData.renderer, texture, NULL, &widget->ren_rect);
    SDL_DestroyTexture(texture);
    return REFRESH;
}


Widget * DebugInfo(char *id, int x, int y){
    Widget *w;
    w = (Widget *)malloc(sizeof(Widget));
    gui_sdl_init_widget(w);
    w->id = id;
    w->ren_rect.x = x;
    w->ren_rect.y = y;
    w->toUpdate = 1;
    w->updateSurface = &debug_theme;
    gui_sdl_add_widget(w);
    return w;
}