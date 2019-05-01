#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "input.h"
#include "console.h"
#include "SDL.h"
#include "SDL_image.h"
#include "point2.h"
#include "math.h"
#include "world.h"
#include "vector2.h"
#include "utility.h"
#include "list.h"

#include "gfx.h"

#define TEXTURE_SIZE_X 128
#define TEXTURE_SIZE_Y 128

#define SKYBOX_SIZE_X 640
#define SKYBOX_SIZE_Y 120

#define TEX_ID_SIZE 64

extern SDL_Surface * screen;

extern float current_fps;

extern bool show_fps;
extern bool show_map;

extern LEVEL loaded_level;

extern VECTOR2 player_pos;
extern float player_pos_height;
extern int current_player_sector;

extern float player_facing;

extern float player_angle_cos;
extern float player_angle_sin;

float map_scale = 20.0f;

char buffer[128];

char * default_font_location;

float * z_buffer;

float hither_z = 1e-4f;
float hither_x;
float hither_y;

float yon_z = 16.0f;
float yon_x;
float yon_y;

float depth_lighting_max_distance = 6.0f;

float hfov = PI/4;
float vfov;

float camera_parameters_x;
float camera_parameters_y;

GFX_TEXTURE loaded_textures[TEX_ID_SIZE];

GFX_TEXTURE_PARAM default_texture;

void GFX_load_texture(char* location, int tex_id)
{
	GFX_TEXTURE new_texture;

	SDL_Surface * tex_surface;

	tex_surface = IMG_Load(location);

	if(tex_surface == NULL)
		printf("Could not load texture id %i, (File: %s), Error: %s\n", tex_id, location, SDL_GetError());

	loaded_textures[tex_id].surface = tex_surface;

	loaded_textures[tex_id].size_x = tex_surface->w;
	loaded_textures[tex_id].size_y = tex_surface->h;
}

void GFX_unload_texture(int tex_id)
{
	SDL_FreeSurface(loaded_textures[tex_id].surface);
}

void set_z_buffer(int x, int y, float value)
{
	z_buffer[x + (y*SCREEN_RES_X)] = value;
}

float get_z_buffer(int x, int y)
{
	return z_buffer[x + y*SCREEN_RES_X];
}

void clear_z_buffer()
{
	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		for(int y = 0; y < SCREEN_RES_Y; y++)
		{
			set_z_buffer(x, y, yon_z);
		}
	}
}

void GFX_Init()
{
	z_buffer = (float *)malloc(sizeof(float) * SCREEN_RES_X * SCREEN_RES_Y);
	clear_z_buffer();

	hither_x = tan(hfov) * hither_z;
	hither_y = (hither_x * SCREEN_RES_Y)/SCREEN_RES_X;

	yon_x = tan(hfov) * yon_z;
	yon_y = yon_x/ASPECT_RATIO;

	vfov = atan(hither_y/hither_z);

	camera_parameters_x = ((hither_z * (float)SCREEN_RES_X/2) / hither_x);
	camera_parameters_y = ((hither_z * (float)SCREEN_RES_Y/2) / hither_y);
	
	GFX_load_font("8x8Font.fnt");

	GFX_load_texture("ground.png", 3);
	GFX_load_texture("wall.png", 0);
	GFX_load_texture("skybox.png", 1);
	GFX_load_texture("terminator.png", 2);

	default_texture.id = 0.;
	default_texture.parallax = 0.;

	default_texture.u_offset = 0.;
	default_texture.v_offset = 0.;

	default_texture.u_scale = 1.;
	default_texture.v_scale = 1.;
}

void GFX_Quit()
{
	free(z_buffer);

	for(int i = 0; i < TEX_ID_SIZE; i++)
	{
		GFX_unload_texture(i);
	}
}

unsigned int GFX_get_pixel(SDL_Surface* surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
   
   	unsigned char *p = (unsigned char *)surface->pixels + y*surface->pitch + x*bpp;

    switch(bpp) {
	    case 1:
	        return *p;
	        break;

	    case 2:
	        return *(unsigned short int *)p;
	        break;

	    case 3:
	        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	        {
	        	return p[0] << 16 | p[1] << 8 | p[2];
	        }
	        else
	        { 
	        	return p[0] | p[1] << 8 | p[2] << 16;
	        }
	        break;

	    case 4:
     		return *(unsigned int *)p;
	       	break;

	    default:
	        return 0;
    }
}

void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel, int transparency)
{
	if(x >= 0 && x < SCREEN_RES_X && y >= 0 && y < SCREEN_RES_Y)
	{	
	    int bpp = surface->format->BytesPerPixel;
	
	    unsigned char *p_base = (unsigned char *)surface->pixels + (y*PIXEL_SCALE)*surface->pitch + (x*PIXEL_SCALE)*bpp;
	    unsigned char *p;
	
	    for(int i = 0; i < PIXEL_SCALE; i ++)
	    {
	    	for(int j = 0; j < PIXEL_SCALE; j ++)
	    	{
	    		p = p_base + i * bpp + j * SCREEN_RES_X * PIXEL_SCALE * bpp;
				
	    		switch(bpp) {
			    case 1:
			        *p = pixel;
			        break;
	
			    case 2:
			        *(unsigned short int *)p = pixel;
			        break;
	
			    case 3:
			        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			            p[0] = (pixel >> 16) & 0xff;
			            p[1] = (pixel >> 8) & 0xff;
			            p[2] = pixel & 0xff;
			        } else {
			            p[0] = pixel & 0xff;
			            p[1] = (pixel >> 8) & 0xff;
			            p[2] = (pixel >> 16) & 0xff;
			        }
			        break;
	
			    case 4:
			    	if(transparency)
			    	{
			    		if(!(((pixel & 0xff) == 255)&& (((pixel >> 8) & 0xff) == 0) && (((pixel >> 16) & 0xff) == 255)))
						{
							p[2] = pixel & 0xff;
				            p[1] = (pixel >> 8) & 0xff;
				            p[0] = (pixel >> 16) & 0xff;
						}	
			    	}
			    	else
			    	{
			    		p[2] = pixel & 0xff;
			            p[1] = (pixel >> 8) & 0xff;
			            p[0] = (pixel >> 16) & 0xff;
			    	}
			        break;
			    }
	    	}
	    }    
	}
}

void GFX_draw_vert_line(SDL_Surface *surface, int x, int y1, int y2, unsigned int pixel)
{
	int temp;

	if(y1 > y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	for(int y = y1; y <= y2; y++)
	{
		GFX_set_pixel(surface, x, y, pixel, 1);
	}
}

void GFX_draw_hor_line(SDL_Surface *surface, int x1, int x2, int y, unsigned int pixel)
{
	int temp;

	if(x1 > x2)
	{
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	for(int x = x1; x <= x2; x++)
	{
		GFX_set_pixel(surface, x, y, pixel, 1);
	}
}

void GFX_draw_line(SDL_Surface *surface, POINT2 p1, POINT2 p2, unsigned int pixel)
{
	float slope;

	POINT2 temp;

	float current_error = 0.0f;

	float y;

	if(p1.x > p2.x)
	{
		temp = p1;
		p1 = p2;
		p2 = temp;
	}

	y = p1.y;

	if(p1.x == p2.x)
	{
		GFX_draw_vert_line(surface, p1.x, p1.y, p2.y, pixel);
	}

	if(p1.y == p2.y)
	{
		GFX_draw_hor_line(surface, p1.x, p2.x, p1.y, pixel);
	}

	slope = (float)(p2.y - p1.y)/(float)(p2.x - p1.x);

	for(int x = p1.x; x <= p2.x; x++)
	{
		GFX_set_pixel(surface, x, y, pixel, 1);
		current_error += slope;
		if(current_error >= 0.5f)
		{
			y = y + 1;
			current_error -= 1.0f;
		}

		if(current_error <= -0.5f)
		{
			y = y - 1;
			current_error += 1.0f;
		}
	}
}

void GFX_set_pixel_from_texture(SDL_Surface *surface,
								GFX_TEXTURE_PARAM texture,
								int screen_x, int screen_y,
								int text_x, int text_y)
{
	int u;
	int v;

	int text_size_x = loaded_textures[texture.id].size_x;
	int text_size_y = loaded_textures[texture.id].size_y;

	u = (text_x / texture.u_scale) - texture.u_offset;
	v = (text_y / texture.v_scale) - texture.v_offset;
	
	if(u >= text_size_x) u = u % text_size_x;
	if(v >= text_size_y) v = v % text_size_y;

	if(u < 0) u = (u % text_size_x) + text_size_x;
	if(v < 0) v = (v % text_size_y) + text_size_y;

	u = clamp_int(u, text_size_x-1, 0);
	v = clamp_int(v, text_size_y-1, 0);

	GFX_set_pixel(surface, screen_x, screen_y, GFX_get_pixel(	loaded_textures[texture.id].surface, 
																u, 
																v), 1);
}

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel)
{
	for(int i = start.x; i <= end.x; i++)
	{
		for(int j = start.y; j <= end.y; j++)
		{
			GFX_set_pixel(screen, i, j, pixel, 1);
		}
	}
}

void GFX_clear_screen()
{

	GFX_fill_rectangle(point2(0,0) , point2(319, 239), SDL_MapRGB(screen->format, 0, 0, 0));
}

void GFX_Draw_Editor()
{
	int last_v;
	unsigned int color;

	VECTOR2 last_v_vector;
	VECTOR2 current_v_vector;

	POINT2 current_line_start;
	POINT2 current_line_end;

	for(int s = 0; s < loaded_level.s_num; s++)
	{
		for(int e = 0; e < (loaded_level.sectors+s)->e_num; e++)
		{
			EDGE current_edge = *((loaded_level.sectors+s)->e + e);
			
			last_v_vector = *(loaded_level.vertexes + current_edge.v_start);
			current_v_vector = *(loaded_level.vertexes + current_edge.v_end);

			last_v_vector = sub_v2(last_v_vector, editor_center);
			current_v_vector = sub_v2(current_v_vector, editor_center);

			current_line_start = point2((int)(last_v_vector.x*editor_zoom) + SCREEN_RES_X/2, SCREEN_RES_Y/2 - (int)(last_v_vector.y*editor_zoom));
			current_line_end = point2((int)(current_v_vector.x*editor_zoom) + SCREEN_RES_X/2, SCREEN_RES_Y/2 - (int)(current_v_vector.y*editor_zoom));

			if(current_edge.is_portal)
			{
				color = SDL_MapRGB(screen->format, 60, 60, 60);
			}
			else
			{
				color = SDL_MapRGB(screen->format, 180, 180, 180);
			}

			GFX_draw_line(screen, current_line_start, current_line_end, color);
		}
	}
}

void GFX_Render()
{
	if(SDL_MUSTLOCK(screen))
	{
		if(SDL_LockSurface(screen) < 0)
		{
			printf("Couldnt lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	GFX_clear_screen();

	GFX_render_3d();

	if(show_map)
	{
		GFX_set_pixel(screen, SCREEN_RES_X/2, SCREEN_RES_Y/2, SDL_MapRGB(screen->format, 255, 0, 0), 1);
		GFX_draw_map();
	}
	
	if(get_console_open())
	{
		GFX_draw_console();	
	}

	if(show_fps)
	{
		sprintf(buffer, "%f", current_fps);
		GFX_draw_string(point2(0, 0), buffer, SDL_MapRGB(screen->format, 255, 255, 0));
	}

	if(SDL_MUSTLOCK(screen))
	{
		SDL_UnlockSurface(screen);
	}

	SDL_UpdateRect(screen, 0, 0, SCREEN_RES_X * PIXEL_SCALE, SCREEN_RES_Y * PIXEL_SCALE);
}

void GFX_load_font(const char * location)
{
	default_font_location = malloc(sizeof(char) * 768);

	FILE * font_file;

	font_file = fopen(location, "r");

	for(int i = 0; i < 768; i++)
	{
		*(default_font_location + i) = fgetc(font_file);
	}

	fclose(font_file);
}

void GFX_draw_char(POINT2 position, char character, unsigned int pixel)
{
	char corrected_char_index;

	char bit_mask = 0x00;

	corrected_char_index = character - 32;

	for(int line = 0; line < 8; line++)
	{
		for(int column = 0; column < 8; column++)
		{
			bit_mask = 0x80 >> column;

			if(bit_mask & *(default_font_location + line + corrected_char_index * 8))
			{
				GFX_set_pixel(screen, position.x + column, position.y + line, pixel, 1);
			}
		}
	}
}

void GFX_draw_string(POINT2 position, char* string, unsigned int pixel)
{
	for(int i = 0; i < 256; i++)
	{
		if(*(string + i) == '\0')
		{
			break;
		}
		
		GFX_draw_char(point2(position.x + i * 8, position.y), *(string + i), pixel);
	}
}

void GFX_draw_console()
{
	GFX_fill_rectangle(point2(0, 0), point2(319, 79), SDL_MapRGB(screen->format, 40, 40, 40));
	GFX_fill_rectangle(point2(0, 80), point2(319, 87), SDL_MapRGB(screen->format, 60, 60, 60));

	for(int y = 0; y < 10; y ++)
	{
		GFX_draw_string(point2(0, 72 - y * 8), get_console_history(y), SDL_MapRGB(screen->format, 200, 200, 200));
		GFX_draw_string(point2(0, 80), get_console_buffer(), SDL_MapRGB(screen->format, 255, 255, 255));
	}
}

void GFX_draw_map()
{
	int last_v;
	unsigned int color;

	VECTOR2 last_v_vector;
	VECTOR2 current_v_vector;

	POINT2 current_line_start;
	POINT2 current_line_end;

	for(int s = 0; s < loaded_level.s_num; s++)
	{
		for(int e = 0; e < (loaded_level.sectors+s)->e_num; e++)
		{
			EDGE current_edge = *((loaded_level.sectors+s)->e + e);
			
			last_v_vector = *(loaded_level.vertexes + current_edge.v_start);
			current_v_vector = *(loaded_level.vertexes + current_edge.v_end);

			last_v_vector = sub_v2(last_v_vector, player_pos);
			current_v_vector = sub_v2(current_v_vector, player_pos);

			current_line_start = point2((int)(last_v_vector.x*map_scale) + SCREEN_RES_X/2, SCREEN_RES_Y/2 - (int)(last_v_vector.y*map_scale));
			current_line_end = point2((int)(current_v_vector.x*map_scale) + SCREEN_RES_X/2, SCREEN_RES_Y/2 - (int)(current_v_vector.y*map_scale));

			if(current_edge.is_portal)
			{
				color = SDL_MapRGB(screen->format, 60, 60, 60);
			}
			else
			{
				color = SDL_MapRGB(screen->format, 180, 180, 180);
			}

			GFX_draw_line(screen, current_line_start, current_line_end, color);
		}
	}
}

COLOR GFX_Color(int r, int g, int b)
{
	COLOR new_color;

	new_color.r = clamp_int(r, 255, 0);
	new_color.g = clamp_int(g, 255, 0);
	new_color.b = clamp_int(b, 255, 0);

	return new_color;
}

COLOR GFX_Color_scale(COLOR color, float factor)
{
	COLOR new_color;

	new_color = GFX_Color(	(int)((float)color.r * factor),
							(int)((float)color.g * factor),
							(int)((float)color.b * factor)    );

	return new_color;
}

unsigned int GFX_Map_Color(COLOR color)
{

	return SDL_MapRGB(screen->format, color.r, color.g, color.b);
}

VECTOR2 convert_ss_to_ws(POINT2 screen_space, float height)
{
	VECTOR2 world_space;

	world_space.y = -(((float)(height)) * camera_parameters_y)/(float)(screen_space.y - SCREEN_RES_Y/2);
	world_space.x = ((float)(screen_space.x - SCREEN_RES_X/2) * world_space.y) / camera_parameters_x;

	world_space = rot_v2(world_space, -player_facing);
	world_space = sum_v2(world_space, player_pos);

	return world_space;
}

POINT2 convert_ws_to_ss(VECTOR2 world_space, float height)
{
	VECTOR2 transformed_pos;
	float transformed_height;

	POINT2 screen_space;

	transformed_pos = sub_v2(world_space, player_pos);
	transformed_pos = rot_v2(transformed_pos, player_facing);

	transformed_height = height - player_pos_height;

	float xscale = camera_parameters_x / transformed_pos.y;
	float yscale = camera_parameters_y / transformed_pos.y;

	float proj_x = transformed_pos.x * xscale;
	float proj_height = transformed_height * yscale;

	int x = (int)(proj_x) + SCREEN_RES_X/2;
	int y = SCREEN_RES_Y/2 - (int)(proj_height);

	screen_space.x = x;
	screen_space.y = y;

	return screen_space;
}

void convert_ws_to_rs(	VECTOR2 world_space, float world_height,
						VECTOR2 * relative_space, float * relative_height)
{
	*relative_space = sub_v2(world_space, player_pos);
	*relative_space = rot_v2(*relative_space, player_facing);

	*relative_height = world_height - player_pos_height;
}

POINT2 convert_rs_to_ss(VECTOR2 relative_space, float relative_height)
{
	POINT2 screen_space;

	float xscale = camera_parameters_x / relative_space.y;
	float yscale = camera_parameters_y / relative_space.y;

	float proj_x = relative_space.x * xscale;
	float proj_height = relative_height * yscale;

	int x = (int)(proj_x) + SCREEN_RES_X/2;
	int y = SCREEN_RES_Y/2 - (int)(proj_height);

	screen_space.x = x;
	screen_space.y = y;

	return screen_space;
}

void GFX_draw_sprite(VECTOR2 sprite_position, VECTOR2 sprite_size, float height)
{
	VECTOR2 transformed_pos;
	GFX_TEXTURE_PARAM texture;

	texture.id = 2;

	texture.parallax = 0;

	texture.u_offset = 0;
	texture.v_offset = 0;

	texture.u_scale = 1.;
	texture.v_scale = 1.;

	float sprite_height;

	transformed_pos = sub_v2(sprite_position, player_pos);
	transformed_pos = rot_v2(transformed_pos, player_facing);

	float transformed_height = height - player_pos_height;

	float xscale = camera_parameters_x / transformed_pos.y;
	float yscale = camera_parameters_y / transformed_pos.y;

	float proj_x = transformed_pos.x * xscale;
	float proj_height = transformed_height * yscale;

	float proj_sprite_size_x = sprite_size.x * xscale;
	float proj_sprite_size_y = sprite_size.y * yscale;

	int ss_sprite_x = (int)(proj_x) + SCREEN_RES_X/2;
	int ss_sprite_y = SCREEN_RES_Y/2 - (int)(proj_height);
	int ss_sprite_size_x = (int)proj_sprite_size_x;
	int ss_sprite_size_y = (int)proj_sprite_size_y;

	int screen_x0 = ss_sprite_x - ss_sprite_size_x/2;
	int screen_x1 = ss_sprite_x + ss_sprite_size_x/2;

	int screen_y0 = ss_sprite_y - ss_sprite_size_y;
	int screen_y1 = ss_sprite_y;

	int c_screen_x0 = clamp_int(screen_x0, SCREEN_RES_X, 0);
	int c_screen_x1 = clamp_int(screen_x1, SCREEN_RES_X, 0);

	int c_screen_y0 = clamp_int(screen_y0, SCREEN_RES_Y, 0);
	int c_screen_y1 = clamp_int(screen_y1, SCREEN_RES_Y, 0);

	for(int x = c_screen_x0; x < c_screen_x1; x++)
	{
		for(int y = c_screen_y0; y < c_screen_y1; y++)
		{
			if(transformed_pos.y < get_z_buffer(x, y))
			{
				GFX_set_pixel_from_texture(	screen, texture, x, y,
											(int)((float)(x-screen_x0)/(float)(screen_x1-screen_x0) * 128),
											(int)((float)(y-screen_y0)/(float)(screen_y1-screen_y0) * 256));
			}
		}
	}
}

/*
{
	void sort3y_p2_uv(POINT2 * points, VECTOR2 * uvs)
	{
		if(points[0].y > points[1].y)
		{	
			swap_p2(&points[0], &points[1]);
			swap_v2(&uvs[0], &uvs[1]);
		}

		if(points[1].y > points[2].y)
		{	
			swap_p2(&points[1], &points[2]);
			swap_v2(&uvs[1], &uvs[2]);
		}

		if(points[0].y > points[1].y)
		{	
			swap_p2(&points[0], &points[1]);
			swap_v2(&uvs[0], &uvs[1]);
		}
	}

	void GFX_texture_tri(	VECTOR2 relative_pos0, VECTOR2 uv0, float height0,
							VECTOR2 relative_pos1, VECTOR2 uv1, float height1,
							VECTOR2 relative_pos2, VECTOR2 uv2, float height2)
	{
		GFX_TEXTURE_PARAM texture;

		texture.id = 0;

		texture.parallax = 0;

		texture.u_offset = 0;
		texture.v_offset = 0;

		texture.u_scale = 1.;
		texture.v_scale = 1.;

		POINT2 ss[3];
		VECTOR2 uv[3];

		float z[3];

		z[0] = relative_pos0.y;
		z[1] = relative_pos1.y;
		z[2] = relative_pos2.y;

		ss[0] = convert_rs_to_ss(relative_pos0, height0);
		ss[1] = convert_rs_to_ss(relative_pos1, height1);
		ss[2] = convert_rs_to_ss(relative_pos2, height2);

		uv[0] = uv0;
		uv[1] = uv1;
		uv[2] = uv2;

		sort3y_p2_uv(ss, uv);

		int Adx, Ady;
		int Bdx, Bdy;
		float slopeA, slopeB;

		int Ax0, Ay0, Ax1, Ay1;
		int Bx0, By0, Bx1, By1;

		float Au0, Av0, Au1, Av1;
		float Bu0, Bv0, Bu1, Bv1;

		float Az0, Az1;
		float Bz0, Bz1;

		if(point_side_v2(p2v2(ss[1]), p2v2(ss[2]), p2v2(ss[0])) == 1)
		{
			Ax0 = ss[0].x; Ax1 = ss[2].x;
			Ay0 = ss[0].y; Ay1 = ss[2].y;

			Bx0 = ss[0].x; Bx1 = ss[1].x;
			By0 = ss[0].y; By1 = ss[1].y;
			//////////////
			Au0 = uv[0].x; Au1 = uv[2].x;
			Av0 = uv[0].y; Av1 = uv[2].y;

			Bu0 = uv[0].x; Bu1 = uv[1].x;
			Bv0 = uv[0].y; Bv1 = uv[1].y;

			Az0 = z[0]; Az1 = z[2];
			Bz0 = z[0]; Bz1 = z[1];
		}
		else
		{
			Ax0 = ss[0].x; Ax1 = ss[1].x;
			Ay0 = ss[0].y; Ay1 = ss[1].y;

			Bx0 = ss[0].x; Bx1 = ss[2].x;
			By0 = ss[0].y; By1 = ss[2].y;
			//////////////
			Au0 = uv[0].x; Au1 = uv[1].x;
			Av0 = uv[0].y; Av1 = uv[1].y;

			Bu0 = uv[0].x; Bu1 = uv[2].x;
			Bv0 = uv[0].y; Bv1 = uv[2].y;

			Az0 = z[0]; Az1 = z[1];
			Bz0 = z[0]; Bz1 = z[2];
		}

		Adx = Ax1 - Ax0;
		Ady = Ay1 - Ay0;

		Bdx = Bx1 - Bx0;
		Bdy = By1 - By0;

		slopeA = (float)(Adx)/(float)(Ady);
		slopeB = (float)(Bdx)/(float)(Bdy);

		int start_y;
		int end_y;

		start_y = clamp_int(ss[0].y, SCREEN_RES_Y, 0);
		end_y = clamp_int(ss[2].y, SCREEN_RES_Y, 0);

		for(int y = start_y; y < end_y; y++)
		{
			if(y == ss[1].y)
			{
				if(point_side_v2(p2v2(ss[1]), p2v2(ss[2]), p2v2(ss[0])) == 1)
				{
					Ax0 = ss[0].x; Ax1 = ss[2].x;
					Ay0 = ss[0].y; Ay1 = ss[2].y;

					Bx0 = ss[1].x; Bx1 = ss[2].x;
					By0 = ss[1].y; By1 = ss[2].y;
					//////////////
					Au0 = uv[0].x; Au1 = uv[2].x;
					Av0 = uv[0].y; Av1 = uv[2].y;

					Bu0 = uv[1].x; Bu1 = uv[2].x;
					Bv0 = uv[1].y; Bv1 = uv[2].y;

					Az0 = z[0]; Az1 = z[2];
					Bz0 = z[1]; Bz1 = z[2];
				}
				else
				{
					Ax0 = ss[1].x; Ax1 = ss[2].x;
					Ay0 = ss[1].y; Ay1 = ss[2].y;

					Bx0 = ss[0].x; Bx1 = ss[2].x;
					By0 = ss[0].y; By1 = ss[2].y;
					//////////////
					Au0 = uv[1].x; Au1 = uv[2].x;
					Av0 = uv[1].y; Av1 = uv[2].y;

					Bu0 = uv[0].x; Bu1 = uv[2].x;
					Bv0 = uv[0].y; Bv1 = uv[2].y;

					Az0 = z[1]; Az1 = z[2];
					Bz0 = z[0]; Bz1 = z[2];
				}
					
				Adx = Ax1 - Ax0;
				Ady = Ay1 - Ay0;

				Bdx = Bx1 - Bx0;
				Bdy = By1 - By0;

				slopeA = (float)(Adx)/(float)(Ady);
				slopeB = (float)(Bdx)/(float)(Bdy);
			}

			float Arelative_y = (float)(y-Ay0)/(float)(Ady);
			float Brelative_y = (float)(y-By0)/(float)(Bdy);

			int x_start = (int)(slopeA * (float)(y - Ay0)) + Ax0;
			int x_end = (int)(slopeB * (float)(y - By0)) + Bx0;

			float t_step = 1./(float)(x_end - x_start);

			float u_start = Arelative_y * (Au1 - Au0) + Au0;
			float v_start = Arelative_y * (Av1 - Av0) + Av0;
			float z_start = Arelative_y * (Az1 - Az0) + Az0;
			float u_end = Brelative_y * (Bu1 - Bu0) + Bu0;
			float v_end = Brelative_y * (Bv1 - Bv0) + Bv0;
			float z_end = Brelative_y * (Bz1 - Bz0) + Bz0;

			VECTOR2 uv_step = scale_v2(vector2(u_end - u_start, v_end - v_start), t_step);

			float z_step = (z_end - z_start) * t_step;

			VECTOR2 current_uv;
			float current_z;

			VECTOR2 start_uv = vector2(u_start, v_start);

			int c_x_start = clamp_int(x_start, SCREEN_RES_X, 0);
			int c_x_end = clamp_int(x_end, SCREEN_RES_X, 0);

			for(int x = c_x_start; x < c_x_end; x++)
			{
				current_uv = sum_v2(start_uv, scale_v2(uv_step, (float)x-x_start));
				current_z = z_start + z_step * (x-x_start);

				if(x == 160)
					printf("%f\n", current_z);

				GFX_set_pixel_from_texture(	screen, texture, x, y,
											(int)(current_uv.x*current_z * 128.),
											(int)(current_uv.y*current_z * 128.) );

			}		
		}
	}

	int GFX_clip_tri(	VECTOR2 * vertexes, VECTOR2 * uvs, float * heights,
						VECTOR2 * clipped_vertexes, VECTOR2 * clipped_uvs, float * clipped_heights)
	{
		//Clips a triangle with the hither z clipping plane, and gives the uvs and heights of every new vertex.
		//Also returns 3 or 4, depending ou how many vertexes got returned

		int poly_size = 0;
		for(int i = 0; i < 3; i ++)
		{
			int next_element = i + 1;
			if(next_element >= 3) next_element = 0;

			if(vertexes[i].y >= hither_z && vertexes[next_element].y >= hither_z)
			{
				clipped_vertexes[poly_size] = vertexes[next_element];
				clipped_uvs[poly_size] = uvs[next_element];
				clipped_heights[poly_size] = heights[next_element];
				poly_size += 1;
			}
			else if(vertexes[i].y < hither_z && vertexes[next_element].y >= hither_z)
			{	
				float relative_x;
				VECTOR2 intersect = intersect_v2(vector2(-10., hither_z), vector2(10., hither_z), vertexes[i], vertexes[next_element]);
				relative_x = (intersect.x - vertexes[i].x)/(vertexes[next_element].x - vertexes[i].x);
				VECTOR2 i_uv = sum_v2(scale_v2(sub_v2(uvs[next_element], uvs[i]), relative_x), uvs[i]);
				float i_height = relative_x * (heights[next_element] - heights[i]) + heights[i];

				clipped_vertexes[poly_size] = intersect;
				clipped_uvs[poly_size] = i_uv;
				clipped_heights[poly_size] = i_height;

				clipped_vertexes[poly_size+1] = vertexes[next_element];
				clipped_uvs[poly_size+1] = uvs[next_element];
				clipped_heights[poly_size+1] = heights[next_element];

				poly_size += 2;
			}
			else if(vertexes[i].y >= hither_z && vertexes[next_element].y < hither_z)
			{
				float relative_x;
				VECTOR2 intersect = intersect_v2(vector2(-10., hither_z), vector2(10., hither_z), vertexes[i], vertexes[next_element]);
				relative_x = (intersect.x - vertexes[i].x)/(vertexes[next_element].x - vertexes[i].x);
				VECTOR2 i_uv = sum_v2(scale_v2(sub_v2(uvs[next_element], uvs[i]), relative_x), uvs[i]);
				float i_height = relative_x * (heights[next_element] - heights[i]) + heights[i];

				clipped_vertexes[poly_size] = intersect;
				clipped_uvs[poly_size] = i_uv;
				clipped_heights[poly_size] = i_height;
				poly_size += 1;
			}
		}

		return poly_size;
	}

	void GFX_project_sprite(VECTOR2 pos0, VECTOR2 uv0, float height0, 
							VECTOR2 pos1, VECTOR2 uv1, float height1,
							VECTOR2 pos2, VECTOR2 uv2, float height2, 
							VECTOR2 pos3, VECTOR2 uv3, float height3)
	{
		VECTOR2 tri0_pos[3];
		VECTOR2 tri0_uv[3];
		float tri0_height[3];
		VECTOR2 c_tri0_pos[4];
		VECTOR2 c_tri0_uv[4];
		float c_tri0_height[4];

		VECTOR2 tri1_pos[3];
		VECTOR2 tri1_uv[3];
		float tri1_height[3];
		VECTOR2 c_tri1_pos[4];
		VECTOR2 c_tri1_uv[4];
		float c_tri1_height[4];

		convert_ws_to_rs(pos0, height0, &(tri0_pos[0]), &(tri0_height[0])); 
		convert_ws_to_rs(pos1, height1, &(tri0_pos[1]), &(tri0_height[1]));
		convert_ws_to_rs(pos2, height2, &(tri0_pos[2]), &(tri0_height[2]));

		tri0_uv[0] = uv0;
		tri0_uv[1] = uv1;
		tri0_uv[2] = uv2;

		////////////////////////

		convert_ws_to_rs(pos0, height0, &(tri1_pos[0]), &(tri1_height[0])); 
		convert_ws_to_rs(pos2, height2, &(tri1_pos[1]), &(tri1_height[1]));
		convert_ws_to_rs(pos3, height3, &(tri1_pos[2]), &(tri1_height[2]));

		tri1_uv[0] = uv0;
		tri1_uv[1] = uv2;
		tri1_uv[2] = uv3;
		
		if(GFX_clip_tri(tri0_pos, tri0_uv, tri0_height,
						c_tri0_pos, c_tri0_uv, c_tri0_height) == 3)
		{
			GFX_texture_tri(c_tri0_pos[0], c_tri0_uv[0], c_tri0_height[0],
							c_tri0_pos[1], c_tri0_uv[1], c_tri0_height[1],
							c_tri0_pos[2], c_tri0_uv[2], c_tri0_height[2]);
		}
		else
		{
			GFX_texture_tri(c_tri0_pos[0], c_tri0_uv[0], c_tri0_height[0],
							c_tri0_pos[1], c_tri0_uv[1], c_tri0_height[1],
							c_tri0_pos[2], c_tri0_uv[2], c_tri0_height[2]);

			GFX_texture_tri(c_tri0_pos[0], c_tri0_uv[0], c_tri0_height[0],
							c_tri0_pos[2], c_tri0_uv[2], c_tri0_height[2],
							c_tri0_pos[3], c_tri0_uv[3], c_tri0_height[3]);
		}

		if(GFX_clip_tri(tri1_pos, tri1_uv, tri1_height,
						c_tri1_pos, c_tri1_uv, c_tri1_height) == 3)
		{
			GFX_texture_tri(c_tri1_pos[0], c_tri1_uv[0], c_tri1_height[0],
							c_tri1_pos[1], c_tri1_uv[1], c_tri1_height[1],
							c_tri1_pos[2], c_tri1_uv[2], c_tri1_height[2]);
		}
		else
		{
			GFX_texture_tri(c_tri1_pos[0], c_tri1_uv[0], c_tri1_height[0],
							c_tri1_pos[1], c_tri1_uv[1], c_tri1_height[1],
							c_tri1_pos[2], c_tri1_uv[2], c_tri1_height[2]);

			GFX_texture_tri(c_tri1_pos[0], c_tri1_uv[0], c_tri1_height[0],
							c_tri1_pos[2], c_tri1_uv[2], c_tri1_height[2],
							c_tri1_pos[3], c_tri1_uv[3], c_tri1_height[3]);
		}
		/*
		printf("ctri0 0 x%f y%f u%f v%f h%f\n", c_tri0_pos[0].x, c_tri0_pos[0].y,
							c_tri0_uv[0].x, c_tri0_uv[0].y, c_tri0_height[0]);
		printf("ctri0 1 x%f y%f u%f v%f h%f\n", c_tri0_pos[1].x, c_tri0_pos[1].y,
							c_tri0_uv[1].x, c_tri0_uv[1].y, c_tri0_height[1]);
		printf("ctri0 2 x%f y%f u%f v%f h%f\n", c_tri0_pos[2].x, c_tri0_pos[2].y,
							c_tri0_uv[2].x, c_tri0_uv[2].y, c_tri0_height[2]);
		printf("ctri0 3 x%f y%f u%f v%f h%f\n", c_tri0_pos[3].x, c_tri0_pos[3].y,
							c_tri0_uv[3].x, c_tri0_uv[3].y, c_tri0_height[3]);

		printf("ctri1 0 x%f y%f u%f v%f h%f\n", c_tri1_pos[0].x, c_tri1_pos[0].y,
							c_tri1_uv[0].x, c_tri1_uv[0].y, c_tri1_height[0]);
		printf("ctri1 1 x%f y%f u%f v%f h%f\n", c_tri1_pos[1].x, c_tri1_pos[1].y,
							c_tri1_uv[1].x, c_tri1_uv[1].y, c_tri1_height[1]);
		printf("ctri1 2 x%f y%f u%f v%f h%f\n", c_tri1_pos[2].x, c_tri1_pos[2].y,
							c_tri1_uv[2].x, c_tri1_uv[2].y, c_tri1_height[2]);
		printf("ctri1 3 x%f y%f u%f v%f h%f\n", c_tri1_pos[3].x, c_tri1_pos[3].y,
							c_tri1_uv[3].x, c_tri1_uv[3].y, c_tri1_height[3]);
		*//*
	}
}
*/

float get_view_plane_pos_x(int ssx)
{
	float pos_x = ((float)(ssx - SCREEN_RES_X/2)/(float)(SCREEN_RES_X/2)) *  hither_x;

	return pos_x;
}

float get_view_angle_from_ss(int ssx)
{
	float angle_x = atan(get_view_plane_pos_x(ssx) / hither_z);

	return angle_x;
}

void GFX_render_3d()
{
	SECTOR * current_sector;
	EDGE * current_edge;
	VECTOR2 transformed_pos_0;
	VECTOR2 transformed_pos_1;

	LIST * pending_portals;

	int current_sector_id;

	int start_screen_x;
	int end_screen_x;

	int y_undrawn_top[SCREEN_RES_X], y_undrawn_bot[SCREEN_RES_X];

	clear_z_buffer();

	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		y_undrawn_top[x] = 0;
		y_undrawn_bot[x] = SCREEN_RES_Y; 
	} 

	start_screen_x = 0;
	end_screen_x = SCREEN_RES_X - 1;

	current_sector_id = current_player_sector;

	pending_portals = create_list();

	//Get current sector
	current_sector = loaded_level.sectors + current_sector_id;

	struct item { SECTOR * sector; int start_screen_x, end_screen_x;};

	struct item first_render = {current_sector, start_screen_x, end_screen_x};

	do
	{
		if(len_list(pending_portals) > 0)
		{
			struct item * current_render = (struct item *)pop_list(pending_portals);

			current_sector = current_render->sector;

			start_screen_x = current_render->start_screen_x;
			end_screen_x = current_render->end_screen_x;

			free (current_render);
		}

		for(int e = 0; e < current_sector->e_num; e++)
		{
			current_edge = current_sector->e + e;

			//if(start_screen_x == end_screen_x) continue;

			VECTOR2 edge_v0 = get_vertex_at(current_edge->v_start);
			VECTOR2 edge_v1 = get_vertex_at(current_edge->v_end);

			//Transform current edges to player point of view (player at 0,0)
			transformed_pos_0 = sub_v2(edge_v0, player_pos);
			transformed_pos_1 = sub_v2(edge_v1, player_pos);
			
			transformed_pos_0 = rot_v2(transformed_pos_0, player_facing);
			transformed_pos_1 = rot_v2(transformed_pos_1, player_facing);

			VECTOR2 ni_pos_0 = transformed_pos_0;
			VECTOR2 ni_pos_1 = transformed_pos_1;

			float t_u0 = 0.;
			float t_u1 = (float)(TEXTURE_SIZE_X);

			//If completely behind player, continue from loop
			if(transformed_pos_0.y <= 0 && transformed_pos_1.y <= 0) continue;
	
			//If partially behind player, intersect with player view area
			if(transformed_pos_0.y <= 0 || transformed_pos_1.y)
			{
				if(transformed_pos_0.y <= 0)
				{	
					VECTOR2 i0 = intersect_v2(vector2(-hither_x, hither_z), vector2(-yon_x, yon_z), transformed_pos_0, transformed_pos_1);
					VECTOR2 i1 = intersect_v2(vector2(hither_x, hither_z), vector2(yon_x, yon_z), transformed_pos_0, transformed_pos_1);

					if(i0.y > 0 && i1.y > 0)
					{
						if(i0.y < i1.y)
							transformed_pos_0 = i0;			
						else
							transformed_pos_0 = i1;			
					}
					else if(i0.y > 0)
						transformed_pos_0 = i0;
					else
						transformed_pos_0 = i1;
				}

				if(transformed_pos_1.y <= 0)
				//if(transformed_pos_1.x <= -(hither_x/hither_z)*transformed_pos_1.z)
				{	
					VECTOR2 i0 = intersect_v2(vector2(-hither_x, hither_z), vector2(-yon_x, yon_z), transformed_pos_0, transformed_pos_1);
					VECTOR2 i1 = intersect_v2(vector2(hither_x, hither_z), vector2(yon_x, yon_z), transformed_pos_0, transformed_pos_1);
						
					if(i0.y > 0 && i1.y > 0)
					{
						if(i0.y < i1.y)
							transformed_pos_1 = i0;
						else
							transformed_pos_1 = i1;
					}
					else if(i0.y > 0)
						transformed_pos_1 = i0;
					else
						transformed_pos_1 = i1;	
				}

				t_u0 = (transformed_pos_0.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);
				t_u1 = (transformed_pos_1.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);			
			}

			//Do projection scales
			float xscale0 = camera_parameters_x / transformed_pos_0.y;
			float yscale0 = camera_parameters_y / transformed_pos_0.y;

			float xscale1 = camera_parameters_x / transformed_pos_1.y;
			float yscale1 = camera_parameters_y / transformed_pos_1.y;

			//Transform to pixel locations (and project)

			float proj_x0 = transformed_pos_0.x * xscale0;
			float proj_x1 = transformed_pos_1.x * xscale1;

			int x0 = (int)(proj_x0) + SCREEN_RES_X/2;
			int x1 = (int)(proj_x1) + SCREEN_RES_X/2;

			//If outside screen, get out
			if(x0 >= x1 || x1 < start_screen_x || x0 > end_screen_x) continue;

			//Get relative ceil and floor heights
			float yceil = current_sector->ceiling_height - player_pos_height;
			float yfloor = current_sector->floor_height - player_pos_height;

			//Project and get pixel position, like above.

			float proj_y0_ceiling = yceil * yscale0;
			float proj_y0_floor = yfloor * yscale0;

			float proj_y1_ceiling = yceil * yscale1;
			float proj_y1_floor = yfloor * yscale1;

			int y0ceiling = SCREEN_RES_Y/2 - (int)(proj_y0_ceiling);
			int y0floor = SCREEN_RES_Y/2 - (int)(proj_y0_floor);

			int y1ceiling = SCREEN_RES_Y/2 - (int)(proj_y1_ceiling);
			int y1floor = SCREEN_RES_Y/2 - (int)(proj_y1_floor);

			SECTOR * neighbor_sector;

			float nyceil= 0;
			float nyfloor= 0;

			int ny0ceiling;
			int ny0floor;

			int ny1ceiling;
			int ny1floor;

			if(current_edge->is_portal)
			{
				neighbor_sector = loaded_level.sectors + current_edge->neighbor_sector_id;

				nyceil = neighbor_sector->ceiling_height - player_pos_height;
				nyfloor = neighbor_sector->floor_height - player_pos_height;
			}

			//Do the same for neighboring sectors
			ny0ceiling = SCREEN_RES_Y/2 - (int)(nyceil * yscale0);
			ny0floor = SCREEN_RES_Y/2 - (int)(nyfloor * yscale0);

			ny1ceiling = SCREEN_RES_Y/2 - (int)(nyceil * yscale1);
			ny1floor = SCREEN_RES_Y/2 - (int)(nyfloor * yscale1);

			int x_begin;
			int x_end;

			x_begin = max_int(x0, start_screen_x);
			x_end = min_int(x1, end_screen_x);

			for(int x = x_begin; x < x_end; x++)
			{
				float relative_x = (float)(x-x0)/(float)(x1-x0);

				int screen_y_ceil = relative_x * (y1ceiling - y0ceiling) + y0ceiling;
				int screen_y_floor = relative_x * (y1floor - y0floor) + y0floor;

				int c_screen_y_ceil = clamp_int(screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
				int c_screen_y_floor = clamp_int(screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

				int text_x = (int)((t_u0*((x1-x)*transformed_pos_1.y) + t_u1*((x-x0)*transformed_pos_0.y)) / ((x1-x)*transformed_pos_1.y + (x-x0)*transformed_pos_0.y));
	
				VECTOR2 world_space;
				
				GFX_draw_visplane(	x, y_undrawn_top[x], c_screen_y_ceil,
									1, yceil - 0.02, 
									current_sector->text_param_ceil);

				GFX_draw_visplane(	x, c_screen_y_floor, y_undrawn_bot[x],
									0, yfloor + 0.02, 
									current_sector->text_param_floor);

				if(current_edge->is_portal)
				{
					int n_screen_y_ceil = relative_x * (ny1ceiling - ny0ceiling) + ny0ceiling;
					int n_screen_y_floor = relative_x * (ny1floor - ny0floor) + ny0floor;

					int c_n_screen_y_ceil = clamp_int(n_screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
					int c_n_screen_y_floor = clamp_int(n_screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

					//If wall between 2 sectors ceiling is visible

					if(c_n_screen_y_ceil > c_screen_y_ceil)
					{
						//Draw wall
						GFX_draw_wall(	x, 
										c_screen_y_ceil, screen_y_ceil, 
										c_n_screen_y_ceil, n_screen_y_ceil, 
										x0, x1+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
										current_edge->text_param); 
					}

					y_undrawn_top[x] = clamp_int(max_int(c_screen_y_ceil, c_n_screen_y_ceil), SCREEN_RES_Y-1, y_undrawn_top[x]);

					//If wall between 2 sectors floor is visible
					if(c_n_screen_y_floor < c_screen_y_floor)
					{
						//Draw wall
						GFX_draw_wall(	x, 
										c_n_screen_y_floor, n_screen_y_floor, 
										c_screen_y_floor, screen_y_floor, 
										x0, x1+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
										current_edge->text_param); 
					}

					y_undrawn_bot[x] = clamp_int(min_int(c_screen_y_floor, c_n_screen_y_floor), y_undrawn_bot[x], 0);
				}
				else
				{
					//Draw a normal wall
					GFX_draw_wall(	x, 
									c_screen_y_ceil, screen_y_ceil, 
									c_screen_y_floor, screen_y_floor, 
									x0, x1+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
									current_edge->text_param);
				}
			}

			if(current_edge->is_portal)
			{
				struct item * pending_portal;
				pending_portal = malloc(sizeof(struct item));

				pending_portal->sector = loaded_level.sectors + current_edge->neighbor_sector_id;
				
				pending_portal->start_screen_x = x_begin;
				pending_portal->end_screen_x = x_end;

				append_list(pending_portals, pending_portal);
			}
		}
	}
	while(len_list(pending_portals) > 0);
}

void GFX_draw_wall(	int screen_x, 
					int top_visible, int top_invisible,
					int bot_visible, int bot_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters)
{
	int text_x = (int)((u0*((x1-screen_x)*z1) + u1*((screen_x-x0)*z0)) / ((x1-screen_x)*z1 + (screen_x-x0)*z0));

	float z = (z1 - z0) * ((((screen_x-x0)*z0)) / ((x1-screen_x)*z1 + (screen_x-x0)*z0)) + z0;

	for(int screen_y = top_visible; screen_y < bot_visible; screen_y++)
	{
		if(texture_parameters.parallax)
		{
			int offset = (player_facing)/(2.*PI) * SKYBOX_SIZE_X;
			GFX_set_pixel_from_texture(	screen,
										texture_parameters,
										screen_x, screen_y,
										screen_x + offset, screen_y);
		}
		else
		{
			int text_y = (float)(screen_y - top_invisible)/(float)(bot_invisible - top_invisible) * (TEXTURE_SIZE_Y);

			GFX_set_pixel_from_texture(	screen,
										texture_parameters,
										screen_x, screen_y,
										text_x, text_y);

			set_z_buffer(screen_x, screen_y, z);
		}
	}	
}

void GFX_draw_sprite_wall (	VECTOR2 start_pos, VECTOR2 end_pos,
							float bot_height, float top_height,
							GFX_TEXTURE_PARAM texture_parameters)
{
	//Transform current edges to player point of view (player at 0,0)
	VECTOR2 transformed_pos_0 = sub_v2(start_pos, player_pos);
	VECTOR2 transformed_pos_1 = sub_v2(end_pos, player_pos);
	
	transformed_pos_0 = rot_v2(transformed_pos_0, player_facing);
	transformed_pos_1 = rot_v2(transformed_pos_1, player_facing);

	VECTOR2 ni_pos_0 = transformed_pos_0;
	VECTOR2 ni_pos_1 = transformed_pos_1;

	float t_u0 = 0.;
	float t_u1 = (float)(TEXTURE_SIZE_X);

	//If completely behind player, continue from loop
	if(transformed_pos_0.y <= 0 && transformed_pos_1.y <= 0) return;

	//If partially behind player, intersect with player view area
	if(transformed_pos_0.y <= 0 || transformed_pos_1.y)
	{
		if(transformed_pos_0.y <= 0)
		{	
			VECTOR2 i0 = intersect_v2(vector2(-hither_x, hither_z), vector2(-yon_x, yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(hither_x, hither_z), vector2(yon_x, yon_z), transformed_pos_0, transformed_pos_1);

			if(i0.y > 0 && i1.y > 0)
			{
				if(i0.y < i1.y)
					transformed_pos_0 = i0;			
				else
					transformed_pos_0 = i1;			
			}
			else if(i0.y > 0)
				transformed_pos_0 = i0;
			else
				transformed_pos_0 = i1;
		}

		if(transformed_pos_1.y <= 0)
		//if(transformed_pos_1.x <= -(hither_x/hither_z)*transformed_pos_1.z)
		{	
			VECTOR2 i0 = intersect_v2(vector2(-hither_x, hither_z), vector2(-yon_x, yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(hither_x, hither_z), vector2(yon_x, yon_z), transformed_pos_0, transformed_pos_1);
				
			if(i0.y > 0 && i1.y > 0)
			{
				if(i0.y < i1.y)
					transformed_pos_1 = i0;
				else
					transformed_pos_1 = i1;
			}
			else if(i0.y > 0)
				transformed_pos_1 = i0;
			else
				transformed_pos_1 = i1;	
		}

		t_u0 = (transformed_pos_0.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);
		t_u1 = (transformed_pos_1.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);			
	}

	//Do projection scales
	float xscale0 = camera_parameters_x / transformed_pos_0.y;
	float yscale0 = camera_parameters_y / transformed_pos_0.y;

	float xscale1 = camera_parameters_x / transformed_pos_1.y;
	float yscale1 = camera_parameters_y / transformed_pos_1.y;

	//Transform to pixel locations (and project)

	float proj_x0 = transformed_pos_0.x * xscale0;
	float proj_x1 = transformed_pos_1.x * xscale1;

	int x0 = (int)(proj_x0) + SCREEN_RES_X/2;
	int x1 = (int)(proj_x1) + SCREEN_RES_X/2;

	//If outside screen, get out
	if(x0 >= x1 || x1 < 0 || x0 > SCREEN_RES_X) return;

	//Get relative ceil and floor heights
	float yceil = top_height - player_pos_height;
	float yfloor = bot_height - player_pos_height;

	//Project and get pixel position, like above.

	float proj_y0_ceiling = yceil * yscale0;
	float proj_y0_floor = yfloor * yscale0;

	float proj_y1_ceiling = yceil * yscale1;
	float proj_y1_floor = yfloor * yscale1;

	int y0ceiling = SCREEN_RES_Y/2 - (int)(proj_y0_ceiling);
	int y0floor = SCREEN_RES_Y/2 - (int)(proj_y0_floor);

	int y1ceiling = SCREEN_RES_Y/2 - (int)(proj_y1_ceiling);
	int y1floor = SCREEN_RES_Y/2 - (int)(proj_y1_floor);

	int x_begin;
	int x_end;

	x_begin = max_int(x0, 0);
	x_end = min_int(x1, SCREEN_RES_X);

	for(int x = x_begin; x < x_end; x++)
	{
		float relative_x = (float)(x-x0)/(float)(x1-x0);

		int screen_y_ceil = relative_x * (y1ceiling - y0ceiling) + y0ceiling;
		int screen_y_floor = relative_x * (y1floor - y0floor) + y0floor;

		int c_screen_y_ceil = clamp_int(screen_y_ceil, SCREEN_RES_Y, 0);
		int c_screen_y_floor = clamp_int(screen_y_floor, SCREEN_RES_Y, 0);			

		GFX_draw_wall(	x, 
						c_screen_y_ceil, screen_y_ceil, 
						c_screen_y_floor, screen_y_floor, 
						x0, x1+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
						texture_parameters);

	}
}

void GFX_draw_sprite_wall_db (	VECTOR2 start_pos, VECTOR2 end_pos,
								float bot_height, float top_height,
								GFX_TEXTURE_PARAM texture_parameters)
{
	GFX_draw_sprite_wall(start_pos, end_pos, bot_height, top_height, texture_parameters);
	GFX_draw_sprite_wall(end_pos, start_pos, bot_height, top_height, texture_parameters);
}

void GFX_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters)
{
	VECTOR2 world_space;
	int is_visible = 1;

	if(is_ceiling && visplane_height < 0.) is_visible = 0;
	if((!is_ceiling) && visplane_height > 0.) is_visible = 0;

	if(is_visible)
	{
		for(int screen_y = visible_top; screen_y < visible_bot; screen_y++)
		{
			if(texture_parameters.parallax)
			{
				int offset = (player_facing)/(2.*PI) * SKYBOX_SIZE_X;
				GFX_set_pixel_from_texture(	screen,
											texture_parameters,
											screen_x, screen_y,
											screen_x + offset, screen_y);
			}
			else
			{
				world_space = convert_ss_to_ws(point2(screen_x, screen_y), visplane_height);
				GFX_set_pixel_from_texture(	screen,
											texture_parameters,
											screen_x, screen_y,
											(int)(world_space.x * 128.), (int)(world_space.y * 128.));
		
			}
		}
	}
}