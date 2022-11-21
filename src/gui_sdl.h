#ifndef _GUI_SDL_H_
#define _GUI_SDL_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#define MAX_FRAME_COUNTER 1000
#define REFRESH 1
#define NOREFRESH 0

typedef struct {
    SDL_Window *window;
    int x, y;
    int w, h;
    int scale;
    int resized;
    int moved;
} WindowData;


typedef struct {
    int x, y;
    int x_press, y_press;   /* last mouse down position */
    int down, up;
    int button;     /* SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT */
    int clicks;     /* 1 for single-click, 2 for double-click, etc. (>= SDL 2.0.2) */
} MouseData;


typedef struct {
    // window info
    // WindowData window;
    SDL_Renderer *renderer;
    // event loop
    SDL_Event event;
    int quit;
    // renderer loop
    int fps;
    int frame_counter;
    int level;
    char *focus_id;
    // user action
    MouseData mouse;
    int texting;
    char input;
} EventData;


typedef struct Widget {
    char *id;
    int tabFocus;
    int x, y, h, w;
    char *text;
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Rect ren_rect;
    int (*updateSurface)(struct Widget *);
    int (*updateData)(struct Widget *, EventData);
    SDL_Color fg;
    SDL_Color bg;
    int toUpdate;
    void *data1;
    void *data2;
} Widget;


typedef struct WidgetNode {
    Widget *widget;
    struct WidgetNode *next;
} WidgetNode;

typedef int (*dataCallBack)(Widget *, EventData);

extern TTF_Font *font;
extern EventData UserData;
extern WidgetNode *layout;

int gui_sdl_init(SDL_Window *win);
void gui_sdl_draw();
void gui_sdl_delay();
void gui_sdl_free();


void gui_sdl_init_widget(Widget *);
void gui_sdl_add_widget(Widget*);
void gui_sdl_widget_connect(Widget *, dataCallBack);

#endif