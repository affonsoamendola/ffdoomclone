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

typedef struct GFX_TEXTURE_
{
	SDL_Surface * surface;

	int size_x;
	int size_y;
}
GFX_TEXTURE;

unsigned int GFX_get_pixel(SDL_Surface* surface, int x, int y);
void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel);

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel);

void GFX_load_font(const char * location);
void GFX_load_texture(char * location, int id);

void GFX_draw_char(POINT2 position, char character, unsigned int pixel);
void GFX_draw_string(POINT2 position, char* string, unsigned int pixel);

void GFX_draw_console();
void GFX_draw_map();

void GFX_draw_wall(	int screen_x, 
					int c_height_visible, int c_height_invisible,
					int f_height_visible, int f_height_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters);

void GFX_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters);

void GFX_render_3d();

void GFX_Render();
void GFX_Init();

#endif