#ifndef GFX_H
#define GFX_H

#include "point2.h"
#include "SDL.h"

typedef struct COLOR_
{
	int r;
	int g;
	int b;
}
COLOR;

typedef struct GFX_TEXTURE_PARAM_
{
	int id;

	int parallax;

	int u_offset;
	int v_offset;

	float u_scale;
	float v_scale;
}
GFX_TEXTURE_PARAM;

unsigned int GFX_get_pixel(SDL_Surface* surface, int x, int y);
void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel);

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel);

void GFX_load_font(const char * location);
void GFX_load_texture(char * location, SDL_Surface * texture);

void GFX_draw_char(POINT2 position, char character, unsigned int pixel);
void GFX_draw_string(POINT2 position, char* string, unsigned int pixel);

void GFX_draw_console();
void GFX_draw_map();

void GFX_render_3d();

void GFX_Render();
void GFX_Init();

#endif