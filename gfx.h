#ifndef GFX_H
#define GFX_H

#include "point2.h"
#include "vector2.h"
#include "SDL.h"

typedef struct COLOR_
{
	char r;
	char g;
	char b;
}
COLOR;

typedef struct TINT_
{
	float r;
	float g;
	float b;
}
TINT;

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

#define DEFAULT_TEXTURE_PARAM (GFX_TEXTURE_PARAM){0, 0, 0, 0, 1.0f, 1.0f}

typedef struct GFX_TEXTURE_
{
	int loaded;
	SDL_Surface * surface;

	int size_x;
	int size_y;
}
GFX_TEXTURE;

unsigned int GFX_get_pixel(SDL_Surface* surface, int x, int y);
void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel, int transparency);

float get_z_buffer(int x, int y);
void set_z_buffer(int x, int y, float value);

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel);
void GFX_clear_screen();

void GFX_load_font(const char * location);
void GFX_load_texture(char * location, int id);

void GFX_draw_char(POINT2 position, char character, unsigned int pixel);
void GFX_draw_string(POINT2 position, char* string, unsigned int pixel);

void GFX_draw_line(SDL_Surface *surface, POINT2 p1, POINT2 p2, unsigned int pixel);

void GFX_draw_console();
void GFX_draw_map();

void GFX_draw_hand();

void GFX_Draw_Editor();



void GFX_draw_sprite(VECTOR2 sprite_position, VECTOR2 sprite_size, float height);
void GFX_draw_sprite_wall (	VECTOR2 start_pos, VECTOR2 end_pos,
							float bot_height, float top_height,
							GFX_TEXTURE_PARAM texture_parameters, TINT tint);

void GFX_set_pixel_from_texture(SDL_Surface *surface,
								GFX_TEXTURE_PARAM texture,
								int screen_x, int screen_y,
								int text_x, int text_y);

int GFX_clip_tri(	VECTOR2 * vertexes, VECTOR2 * uvs, float * heights,
					VECTOR2 * clipped_vertexes, VECTOR2 * clipped_uvs, float * clipped_heights);

void GFX_texture_tri(	VECTOR2 pos0, VECTOR2 uv0, float height0,
						VECTOR2 pos1, VECTOR2 uv1, float height1,
						VECTOR2 pos2, VECTOR2 uv2, float height2);
						
void GFX_project_sprite(VECTOR2 pos0, VECTOR2 uv0, float height0, 
						VECTOR2 pos1, VECTOR2 uv1, float height1,
						VECTOR2 pos2, VECTOR2 uv2, float height2, 
						VECTOR2 pos3, VECTOR2 uv3, float height3);

void GFX_draw_wall(	int screen_x, 
					int c_height_visible, int c_height_invisible,
					int f_height_visible, int f_height_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters,
					TINT tint);

void GFX_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters,
						TINT tint);


unsigned int GFX_Scale_Pixel(unsigned int pixel, float scale);
unsigned int GFX_Tint_Pixel(unsigned int pixel, TINT tint);


TINT GFX_Tint(float r, float g, float b);
COLOR GFX_Color(int r, int g, int b);
COLOR GFX_Color_scale(COLOR color, float factor);
unsigned int GFX_Map_Color(COLOR color);

void GFX_render_3d();

void GFX_Render();
void GFX_Init();
void GFX_Quit(); 

#endif