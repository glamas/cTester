
#include "gui_sdl.h"
#include <SDL_syswm.h>


/* extern var init */
EventData UserData = {.quit=0, .frame_counter=0};
WidgetNode *layout = NULL;
// TTF_Font *font = NULL;


/* static var */
static UINT_PTR DRAW_TIMER_ID;
static int frame = 60;
static Uint64 delay_counter = 0;
static SDL_Window *window;
static SDL_Renderer *renderer;
static int font_group_idx = 0;
static FontInfo font_group[MAX_FONT_GROUP] = {0};


static void ren_update_widget_data(EventData eData){
    WidgetNode *p = layout;
    Widget *w = NULL;
    while (p) {
        w = p->widget;
        if ( w->updateData != NULL ) w->updateData(w, eData);
        p = p->next;
    }
}


static void ren_update_event_data(){
    if (UserData.event.type == SDL_MOUSEMOTION){
        UserData.mouse.x = UserData.event.motion.x;
        UserData.mouse.y = UserData.event.motion.y;
    }
    if (UserData.event.type == SDL_MOUSEBUTTONDOWN || UserData.event.type == SDL_MOUSEBUTTONUP){
        UserData.mouse.x = UserData.event.button.x;
        UserData.mouse.y = UserData.event.button.y;
        UserData.mouse.button = UserData.event.button.button;
        UserData.mouse.clicks = UserData.event.button.clicks;
        if (UserData.event.type == SDL_MOUSEBUTTONDOWN){
            UserData.mouse.down = 1;
            UserData.mouse.up = 0;
            if (UserData.mouse.x_press == -1) {
                UserData.mouse.x_press = UserData.mouse.x;
                UserData.mouse.y_press = UserData.mouse.y;
            }
        } else {
            UserData.mouse.down = 0;
            UserData.mouse.up = 1;
            UserData.mouse.x_press = -1;
            UserData.mouse.y_press = -1;
        }
    }
    ren_update_widget_data(UserData);
}


static int ren_update_layout() {
    int refresh = 0;
    WidgetNode *p = layout;
    Widget *w = NULL;
    while (p) {
        w = p->widget;
        // TODO: level, all refresh
        if ( w->updateSurface(w) == REFRESH )
            refresh = 1;
        p = p->next;
    }
    return refresh;
}


void GuiSDL_Draw(){
    float elapsed = 0.0f;
    Uint64 counter = SDL_GetPerformanceCounter();

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    int refresh = ren_update_layout();

    if (refresh == REFRESH) {
        SDL_RenderPresent(renderer);
    }
    if (DRAW_TIMER_ID) {
        elapsed = (float)(((SDL_GetPerformanceCounter() - counter) * 1000.0f) / SDL_GetPerformanceFrequency());
        UserData.fps = (int)(1000 / elapsed);
    }
}


static int ren_event_watch(void* userdata, SDL_Event* event)
{
    UserData.event = *event;
    SDL_SysWMmsg* sys_event = event->syswm.msg;
    switch(event->type) {
        case SDL_QUIT:
            UserData.quit = 1;
            if (DRAW_TIMER_ID) {
                KillTimer(sys_event->msg.win.hwnd, DRAW_TIMER_ID);
                DRAW_TIMER_ID = 0;
            }
            break;
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
            ren_update_event_data();
            break;
        // case SDL_TEXTINPUT:
        //     UserData.input = event->text.text;
        //     printf("%s - %s\n", event->text.text, UserData.input);
        //     break;
        case SDL_KEYDOWN:
            UserData.input = event->key.keysym.sym;
            break;
        case SDL_KEYUP:
            UserData.input = '\0';
            break;
        case SDL_SYSWMEVENT:
            switch (sys_event->msg.win.msg) {
                case WM_ENTERSIZEMOVE:
                case WM_ENTERMENULOOP:
                    DRAW_TIMER_ID = (LONG_PTR)window;
                    SetTimer(sys_event->msg.win.hwnd, DRAW_TIMER_ID, (int)(1000 / frame), NULL);
                    break;
                case WM_TIMER:
                    GuiSDL_Draw();
                    break;
                case WM_SIZE:
                    if (DRAW_TIMER_ID)
                        GuiSDL_Draw();
                    break;
                case WM_EXITSIZEMOVE:
                case WM_EXITMENULOOP:
                    KillTimer(sys_event->msg.win.hwnd, DRAW_TIMER_ID);
                    DRAW_TIMER_ID = 0;
                    break;
            }
            break;
        default:
            break;
    }
    return 0;
}


int GuiSDL_Init(SDL_Window *win) {
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    // assert(win);
    if (SDL_Init(SDL_INIT_EVERYTHING) !=0) {
        SDL_Log("SDL_Init");
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG) == 0) {
        printf("Error SDL2_image Initialization");
        return 2;
    }
    window = win;

    // renderer init
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    UserData.renderer = renderer;

    // Initialize SDL2_ttf
    TTF_Init();
    char *fontPath;
    int fontSize = 20;
#ifdef _WIN32
      fontPath = "c:\\Windows\\Fonts\\simsun.ttc";
#else
      fontPath = "./SourceHanSerif-VF.ttf.ttc";
#endif
    GuiSDL_InitFont(fontPath, "main", fontSize, TTF_STYLE_NORMAL);
    // font = TTF_OpenFont(fontPath, fontSize);
    // if ( ! font ){
    //     SDL_Log("Unable to load font: '%s'!\n"
    //            "SDL2_ttf Error: %s\n", fontPath, TTF_GetError());
    //     return 1;
    // }
    // SDL_StartTextInput();
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    SDL_AddEventWatch(ren_event_watch, NULL);
    return 0;
}


void GuiSDL_Delay() {
    Uint64 nCurrCounter;
    float elapsed = 0.0f;
    nCurrCounter = SDL_GetPerformanceCounter();
    elapsed = (float)(((nCurrCounter - delay_counter) * 1000.0f) / SDL_GetPerformanceFrequency());
    if (elapsed <= 1000 / frame) {
        SDL_Delay((int)(1000 / frame - elapsed));
    }
    nCurrCounter = SDL_GetPerformanceCounter();
    elapsed = (float)(((nCurrCounter - delay_counter) * 1000.0f) / SDL_GetPerformanceFrequency());
    UserData.fps = (int)(1000 / elapsed);
    delay_counter = nCurrCounter;
}


static void free_layout() {
    WidgetNode *p;
    Widget *w = NULL;
    while (layout) {
        p = layout->next;
        w = layout->widget;
        // use SDL2 method to free surface or texture
        if (w->surface != NULL) SDL_FreeSurface(w->surface);
        if (w->texture != NULL) SDL_DestroyTexture(w->texture);
        // use free() to free which create by malloc
        if (w != NULL) free(w);
        if (layout != NULL) free(layout);
        layout = p;
    }
}


void GuiSDL_Free(){
    free_layout();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


/* widget */
void GuiSDL_InitWidget(Widget *w) {
    w->to_update = 1;
    w->surface = NULL;
    w->texture = NULL;
    w->bg.r = 0xFF;
    w->bg.g = 0xFF;
    w->bg.b = 0xFF;
    w->bg.a = 0xFF;
    w->fg.r = 0x00;
    w->fg.g = 0x00;
    w->fg.b = 0x00;
    w->fg.a = 0xFF;
    w->data1 = NULL;
    w->data2 = NULL;
    w->updateSurface = NULL;
    w->updateData = NULL;
}

void GuiSDL_AddWidget(Widget *w) {
    WidgetNode *node, *p;
    node = (WidgetNode *)malloc(sizeof(WidgetNode));
    node->widget = w;
    node->next = NULL;
    if ( layout == NULL ) {
        layout = node;
    } else {
        p = layout;
        while(p->next) {p = p->next;}
        p->next = node;
    }
}

void GuiSDL_connectWidget(Widget *w, dataCallBack fn) {
    w->updateData = fn;
}

/* font */
int GuiSDL_InitFont(const char *path, const char *alias, int ptsize, int style) {
    font_group[font_group_idx].font = TTF_OpenFont(path, ptsize);
    font_group[font_group_idx].alias = alias;
    font_group[font_group_idx].size = ptsize;
    font_group[font_group_idx].style = style;
    font_group[font_group_idx].scale = 1.0;
    font_group_idx += 1;
    return 0;
}
FontInfo * GuiSDL_GetFont(const char *alias) {
    for (int i=0; i < MAX_FONT_GROUP; i++){
        if (0 == strcmp(font_group[i].alias, alias)){
            return &font_group[i];
        }
    }
    return NULL;
}

void GuiSDL_SetFontSize(FontInfo *font, int ptsize) {
    if (font->size != ptsize){
        TTF_SetFontSize(font->font, ptsize);
        font->size = ptsize;
    }
}

void GuiSDL_SetFontStyle(FontInfo *font, int style) {
    if (font->style != style){
        TTF_SetFontStyle(font->font, style);
        font->style = style;
    }
}