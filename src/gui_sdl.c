
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
        if (w->text != NULL) GuiSDL_FreeCharCell(w->text);
        if (w->freeWidget != NULL) w->freeWidget(w);
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
    w->tab_focus = 0;
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
    w->freeWidget = NULL;
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

int GuiSDL_CheckWidgetRedrawOrClean(Widget *w) {
    if (w->to_update == 0) {
        SDL_RenderCopy(UserData.renderer, w->texture, NULL, &w->ren_rect);
        return NOREFRESH;
    }
    if (w->surface) SDL_FreeSurface(w->surface);
    if (w->texture) SDL_DestroyTexture(w->texture);
    return REFRESH;
}

void GuiSDL_ConnectWidget(Widget *w, dataCallBack fn) {
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

// copy from neovim
const int utf8len_tab[] = {
  // ?1 ?2 ?3 ?4 ?5 ?6 ?7 ?8 ?9 ?A ?B ?C ?D ?E ?F
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 1?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 2?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 3?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 4?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 5?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 6?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 7?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 8?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 9?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // A?
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // B?
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // C?
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // D?
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  // E?
  4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1,  // F?
};

static int GuiSDL_U8CharSize(const char *data) {
    if (data == NULL) return 1;
    return utf8len_tab[data[0]];
}


int GuiSDL_SetWidgetText(FontInfo *font_info, Widget *w, const char *text) {
    const char *p;
    int charsize = 0;
    int count = 0;
    CharCell *node = NULL;
    CharCell *cell = NULL;

    if (text == NULL) return count;
    if (w->text != NULL) GuiSDL_FreeCharCell(w->text);

    node = w->text;
    p = text;
    while (*p != '\0') {
        cell = (CharCell *)malloc(sizeof(CharCell));
        charsize = GuiSDL_U8CharSize(p);
        memcpy(cell->ch, p, charsize);
        cell->to_update = 1;
        cell->size = font_info->size;
        cell->style = font_info->style;
        cell->fg = w->fg;
        cell->bg = w->bg;
        if (cell->glyph) SDL_FreeSurface(cell->glyph);
        if (cell->ch == '\t') {
            cell->glyph = TTF_RenderUTF8_Shaded(font_info->font, "  ", cell->fg, cell->bg);
        } else if (cell->ch == '\n') {
            cell->glyph = TTF_RenderGlyph32_Shaded(font_info->font, ' ', cell->fg, cell->bg);
        } else {
            cell->glyph = TTF_RenderGlyph32_Shaded(font_info->font, cell->ch, cell->fg, cell->bg);
        }
        cell->to_update = 0;
        // cell->glyph = TTF_RenderGlyph32_Shaded(font_info->font, cell->ch, w->fg, w->bg);
        if (node == NULL) {
             node = cell;
             node->next = NULL;
        } else {
            node->next = cell;
            node = node->next;
        }
        count += 1;
        p = p + charsize;
    }
    return count;
}

SDL_Surface * GuiSDL_GetWidgetTextSurface(FontInfo *font_info, Widget *w, int wrap_width) {
    if (w->text == NULL) {
        return NULL;
    }
    int linenum = 0;
    int line_max_h = 0;
    int line_max_w = 0;
    int w_surf = 0, h_surf = 0;
    CharCell *node = NULL;
    CharCell *node_line = NULL;
    // 遍历每个CharCell，并计算surface大小
    node = w->text;
    for (; node->next != NULL; node=node->next) {
        if (wrap_width > 0 && line_max_w == 0 && node->glyph->w > wrap_width) {
            // wrap_width too small
            wrap_width = node->glyph->w;
        }
        if ((wrap_width > 0 && (line_max_w + node->glyph->w) >= wrap_width)
            || node->ch == '\n'
            || node->next == NULL
        ) {
            if (line_max_h == 0) h_surf += node->glyph->h;      // new line
            else h_surf += line_max_h;
            line_max_h = 0;
            if (w_surf < line_max_w) w_surf = line_max_w;
            line_max_w = 0;
        }
        if (line_max_h < node->glyph->h) line_max_h = node->glyph->h;
        line_max_w += node->glyph->w;
    }
    // 创建surface，并布局CharCell
    line_max_h = 0;
    line_max_w = 0;
    int destX = 0, destY = 0;
    SDL_Rect rect = {.w=w_surf, .h=w_surf};
    SDL_Rect cell_rect;
    SDL_Surface *surf = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);
    node = w->text;
    node_line = node;
    for (; node->next != NULL; node=node->next) {
        // 遍历。node_line指向每行的第一个，node遍历，当遇到行尾或者换行，将该行写入surface
        if ((line_max_w + node->glyph->w) >= rect.w
            || node->ch == '\n'
            || node->next == NULL
        ) {
            line_max_w = 0;
            // 遍历node_line，写入surface
            while (node_line->next != node->next) {
                cell_rect.w = node_line->glyph->w;
                cell_rect.h = node_line->glyph->h;
                cell_rect.x = destX;
                cell_rect.y = destY + line_max_h - node_line->glyph->h;
                SDL_BlitSurface(node_line->glyph, NULL, surf, &cell_rect);
                destX += node_line->glyph->w;
                node_line = node_line->next;
            }
            if (node->next != NULL) node_line = node->next;
            // else node_line = NULL;
            destX = 0;
            destY += line_max_h;
        }
        if (line_max_h < node->glyph->h) line_max_h = node->glyph->h;
        line_max_w += node->glyph->w;
    }
    return surf;
}

// SDL_Surface * GuiSDL_RenderUTF8_Shaded_Wrapped(FontInfo *font_info, Widget *w) {
//     char *p;
//     int charsize = 0;
//     CharCell *node = NULL;
//     CharCell *node_line = NULL;
//     if (w->text == NULL){
//         return TTF_RenderGlyph32_Shaded(font_info->font, ' ', w->fg, w->bg);
//     }
//     if (w->data2 != NULL) {
//         GuiSDL_FreeCharCell(w->data2);
//         w->data2 = NULL;
//     }
//     int size = font_info->size;
//     int style = font_info->style;
//     // 遍历每个char，根据char生成每个CharCell，生成glyph
//     node = (CharCell *)w->data2;
//     p = w->text;
//     while (*p != '\0') {
//         CharCell *cell = (CharCell *)malloc(sizeof(CharCell));
//         charsize = GuiSDL_U8CharSize(p);
//         memcpy(cell->ch, p, charsize);
//         cell->to_update = 1;
//         cell->size = font_info.size;
//         cell->style = font_info.style;
//         cell->fg = w->fg;
//         cell->bg = w->bg;
//         if (node->glyph) SDL_FreeSurface(node->glyph);
//         if (node->size != font_info->size) {
//             GuiSDL_SetFontSize(font_info, node->size);
//         }
//         if (node->style != font_info->style) {
//             GuiSDL_SetFontStyle(font_info, node->style);
//         }
//         if (node->ch == '\t') {
//             node->glyph = TTF_RenderUTF8_Shaded(font_info->font, "  ", node->fg, node->bg);
//         } else if (node->ch == '\n') {
//             node->glyph = TTF_RenderGlyph32_Shaded(font_info->font, ' ', node->fg, node->bg);
//         } else {
//             node->glyph = TTF_RenderGlyph32_Shaded(font_info->font, ch, node->fg, node->bg);
//         }
//         node->to_update = 0;
//         // cell->glyph = TTF_RenderGlyph32_Shaded(font_info->font, cell->ch, w->fg, w->bg);
//         if (node == NULL) {
//              node = cell;
//              node->next = NULL;
//         } else {
//             node->next = cell;
//             node = node->next;
//         }
//         p = p + charsize;
//     }
//     // set back font
//     GuiSDL_SetFontSize(font_info, size);
//     GuiSDL_SetFontStyle(font_info, style);

//     int linenum = 0;
//     int line_max_h = 0;
//     int line_max_w = 0;
//     int w_surf = 0, h_surf = 0;
//     // 遍历每个CharCell，生成glyph，并计算surface大小
//     node = (CharCell *)w->data2;
//     for (; node->next != NULL; node=node->next) {
//         if (node->ch == '\n') {     // TODO: new line or reach right edge
//             if (line_max_h == 0) h_surf += node->glyph->h;
//             else h_surf += line_max_h;
//             line_max_h = 0;
//             if (w_surf < line_max_w) w_surf = line_max_w;
//             line_max_w = 0;
//         }
//         if (line_max_h < node->glyph->h) line_max_h = node->glyph->h;
//         line_max_w += node->glyph->w;
//     }
//     // 创建surface，并布局CharCell
//     line_max_h = 0;
//     int destX = 0, destY = 0;
//     SDL_Rect rect = {.w=w_surf, .h=w_surf};
//     SDL_Rect cell_rect;
//     SDL_Surface *surf = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);
//     node = (CharCell *)w->data2;
//     node_line = node;
//     for (; node->next != NULL; node=node->next) {
//         for (; node_line->next != NULL && node_line->ch != '\n'; node_line = node_line->next) {
//             cell_rect.h = 0;
//             cell_rect.w = 0;
//         }
//         cell_rect.w = node->glyph->w;
//         cell_rect.h = node->glyph->h;
//         SDL_BlitSurface(node->glyph, NULL, surf, &borderRect);
//     }
// }

void GuiSDL_FreeCharCell(void *data) {
    CharCell *point, *node;
    if (data != NULL) {
        point = (CharCell *)data;
        while (point != NULL) {
            node = point;
            point = (CharCell *)point->next;
            if (node->glyph) SDL_FreeSurface(node->glyph);
            free(node);
        }
    }
}
