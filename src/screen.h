#ifndef H_SCREEN_
#define H_SCREEN_
#include "board.h"
#include <SDL.h>

SDL_Texture *texture;
SDL_Surface *buffer, *screen;
SDL_Renderer *renderer;
SDL_Window *window;
int draw_w, draw_h;

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

void init_screen(void);
void load_assets(void);
void screenspace2gemspace(BOARD *b,int sx,int sy,int *gx,int *gy);
int g2s_x(int x);
int g2s_y(int x,int y);
void flip_screen(void);
void draw_intro(void);
void draw_main_menu(void);
int draw_high_score(int gamemode);
void draw_button(SDL_Rect *r,char *text);
void SDL_textinput(SDL_Surface *font,SDL_Surface * dest,int x,int y,char *string,int size);

#endif
