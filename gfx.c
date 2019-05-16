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
#include "player.h"
#include "ui.h"

#include "gfx.h"

#define TEXTURE_SIZE_X 128
#define TEXTURE_SIZE_Y 128

#define SKYBOX_SIZE_X 640
#define SKYBOX_SIZE_Y 120

#define TEX_ID_SIZE 256

#define HAND_TEX_ID 10

extern SDL_Surface * screen;

extern float current_fps;

extern bool show_fps;
extern bool show_map;

extern LEVEL loaded_level;

extern PLAYER * player;

char buffer[128];

char * default_font_location;

CAMERA * main_camera;

GFX_TEXTURE loaded_textures[TEX_ID_SIZE];

GFX_TEXTURE_PARAM default_texture;

void GFX_load_resource_list(char* location)
{
	FILE * resource_list = fopen(location, "r");

	int tex_id;
	char buffer[128];

	while(fscanf(resource_list, "%i %s", &tex_id, buffer) == 2)
	{
		GFX_load_texture(buffer, tex_id);
	}
}

void GFX_load_texture(char* location, int tex_id)
{
	GFX_TEXTURE new_texture;

	SDL_Surface * tex_surface;

	tex_surface = IMG_Load(location);

	if(tex_surface == NULL)
		printf("Could not load texture id %i, (File: %s), Error: %s\n", tex_id, location, SDL_GetError());

	loaded_textures[tex_id].loaded = 1;
	loaded_textures[tex_id].surface = tex_surface;

	loaded_textures[tex_id].size_x = tex_surface->w;
	loaded_textures[tex_id].size_y = tex_surface->h;
}

void GFX_unload_texture(int tex_id)
{
	loaded_textures[tex_id].loaded = 0;
	SDL_FreeSurface(loaded_textures[tex_id].surface);
}

void set_z_buffer(CAMERA * camera, int x, int y, float value)
{
	camera->z_buffer[x + (y*SCREEN_RES_X)] = value;
}

float get_z_buffer(CAMERA * camera, int x, int y)
{
	return camera->z_buffer[x + y*SCREEN_RES_X];
}

void clear_z_buffer(CAMERA * camera)
{
	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		for(int y = 0; y < SCREEN_RES_Y; y++)
		{
			set_z_buffer(camera, x, y, camera->yon_z);
		}
	}
}

void GFX_Init_Camera(CAMERA ** camera)
{
	CAMERA initted_camera;

	initted_camera.hither_z = 1e-4f;
	initted_camera.yon_z = 16.0f;
	initted_camera.hfov = PI/4;
	initted_camera.depth_lighting_max_distance = 6.0f;

	initted_camera.z_buffer = (float *)malloc(sizeof(float) * SCREEN_RES_X * SCREEN_RES_Y);
	clear_z_buffer(&initted_camera);

	initted_camera.hither_x = tan(initted_camera.hfov) * initted_camera.hither_z;
	initted_camera.hither_y = (initted_camera.hither_x * SCREEN_RES_Y)/SCREEN_RES_X;

	initted_camera.yon_x = tan(initted_camera.hfov) * initted_camera.yon_z;
	initted_camera.yon_y = initted_camera.yon_x/ASPECT_RATIO;

	initted_camera.vfov = atan(initted_camera.hither_y/initted_camera.hither_z);

	initted_camera.camera_parameters_x = ((initted_camera.hither_z * (float)SCREEN_RES_X/2) / initted_camera.hither_x);
	initted_camera.camera_parameters_y = ((initted_camera.hither_z * (float)SCREEN_RES_Y/2) / initted_camera.hither_y);

	*camera = malloc(sizeof(CAMERA));
	**camera = initted_camera;
}

void GFX_Destroy_Camera(CAMERA * camera)
{
	free(camera->z_buffer);
	free(camera);
}

void GFX_Init()
{
	GFX_Init_Camera(&main_camera);
	
	GFX_load_font("8x8Font.fnt");

	default_texture.id = 0;
	default_texture.parallax = 0;

	default_texture.u_offset = 0;
	default_texture.v_offset = 0;

	default_texture.u_scale = 1.;
	default_texture.v_scale = 1.;

	for(int i = 0; i < TEX_ID_SIZE; i++)
	{
		loaded_textures[i].loaded = 0;
	}

	GFX_load_resource_list("graphix/default.rls");

	GFX_load_texture("graphix/coffeehands.png", HAND_TEX_ID);
	GFX_load_texture("graphix/terminator.png", 11);
	GFX_load_texture("graphix/ui.png", UI_TEX_ID);
}

void GFX_Quit()
{
	GFX_Destroy_Camera(main_camera);

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
	        return p[0] << 16 | p[1] << 8 | p[2];
	        break;

	    case 4:
     		return p[0] << 16| p[1] << 8 | p[2] << 0;
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
			    	if(	!(transparency && 
			    		((pixel & 0xff) == 255) && 
			    		(((pixel >> 8 ) & 0xff) == 0) &&
			    		(((pixel >> 16) & 0xff)	== 255) ))
			    	{
			    		p[0] = pixel & 0xff;
		            	p[1] = (pixel >> 8) & 0xff;
		          		p[2] = (pixel >> 16) & 0xff;
			    	}   
			        break;
	
			    case 4:
			    	if(	!(transparency && 
			    		((pixel & 0xff) == 255) && 
			    		(((pixel >> 8 ) & 0xff) == 0) &&
			    		(((pixel >> 16) & 0xff)	== 255) ))
			    	{
			    		p[0] = pixel & 0xff;
		            	p[1] = (pixel >> 8) & 0xff;
		          		p[2] = (pixel >> 16) & 0xff;
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

	float x, y;

	if(p1.x == p2.x)
	{
		GFX_draw_vert_line(surface, p1.x, p1.y, p2.y, pixel);
	}

	if(p1.y == p2.y)
	{
		GFX_draw_hor_line(surface, p1.x, p2.x, p1.y, pixel);
	}

	if(fabs(p2.y - p1.y) < fabs(p2.x - p1.x))
	{
		if(p1.x > p2.x)
		{
			temp = p1;
			p1 = p2;
			p2 = temp;
		}

		y = p1.y;
		
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
	else
	{
		if(p1.y > p2.y)
		{
			temp = p1;
			p1 = p2;
			p2 = temp;
		}

		x = p1.x;
		
		slope = (float)(p2.x - p1.x)/(float)(p2.y - p1.y);

		for(int y = p1.y; y <= p2.y; y++)
		{
			GFX_set_pixel(surface, x, y, pixel, 1);

			current_error += slope;

			if(current_error >= 0.5f)
			{
				x = x + 1;
				current_error -= 1.0f;
			}

			if(current_error <= -0.5f)
			{
				x = x - 1;
				current_error += 1.0f;
			}
		}
	}
}

unsigned int GFX_get_pixel_from_texture(	GFX_TEXTURE_PARAM texture,
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

	return GFX_get_pixel(	loaded_textures[texture.id].surface, u, v);
}

void GFX_set_pixel_from_texture_depth_tint(	SDL_Surface *surface,
											GFX_TEXTURE_PARAM texture,
											int screen_x, int screen_y,
											int text_x, int text_y, 
											float depth, TINT tint)
{
	unsigned int pixel;

	pixel = GFX_get_pixel_from_texture(texture, text_x, text_y);

	float relative_z = (main_camera->depth_lighting_max_distance - depth)/(main_camera->depth_lighting_max_distance - main_camera->hither_z);

	GFX_set_pixel(surface, screen_x, screen_y, 
				  GFX_Tint_Pixel(	GFX_Scale_Pixel(  	pixel, 
				  										relative_z), 
				  					tint),
				  1);
}

void GFX_set_pixel_from_texture_tint(	SDL_Surface *surface,
										GFX_TEXTURE_PARAM texture,
										int screen_x, int screen_y,
										int text_x, int text_y, 
										TINT tint)
{
	unsigned int pixel;

	pixel = GFX_get_pixel_from_texture(texture, text_x, text_y);

	GFX_set_pixel(surface, screen_x, screen_y, 
				  GFX_Tint_Pixel(	pixel, 
				  					tint),
				  1);
}

void GFX_set_pixel_from_texture_depth(	SDL_Surface *surface,
										GFX_TEXTURE_PARAM texture,
										int screen_x, int screen_y,
										int text_x, int text_y, float depth)
{
	unsigned int pixel;

	pixel = GFX_get_pixel_from_texture(texture, text_x, text_y);

	float relative_z = (main_camera->depth_lighting_max_distance - depth)/(main_camera->depth_lighting_max_distance - main_camera->hither_z);

	GFX_set_pixel(surface, screen_x, screen_y, 
				  GFX_Scale_Pixel(  pixel, 
				  					relative_z), 
				  1);
}

void GFX_set_pixel_from_texture(	SDL_Surface *surface,
									GFX_TEXTURE_PARAM texture,
									int screen_x, int screen_y,
									int text_x, int text_y)
{
	unsigned int pixel;

	pixel = GFX_get_pixel_from_texture(texture, text_x, text_y);	

	GFX_set_pixel(surface, screen_x, screen_y, pixel, 1);
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

	GFX_draw_sprite(vector2(0., 2.), vector2(0.35f, 0.7f), 0);

	GFX_draw_hand();

	GFX_draw_ui();

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



void GFX_draw_7_segment(POINT2 position, int value)
{

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

TINT GFX_Tint(float r, float g, float b)
{
	TINT tint;

	tint.r = r;
	tint.g = g;
	tint.b = b;

	return tint;
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

unsigned int GFX_Scale_Pixel(unsigned int pixel, float scale)
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	SDL_GetRGB(pixel, screen->format, &r, &g, &b);

	r = (unsigned char)(((float)r) * scale);
	g = (unsigned char)(((float)g) * scale);
	b = (unsigned char)(((float)b) * scale);

	if(scale <= 0)
	{
		r = 0;
		g = 0;
		b = 0;
	}

	unsigned int scaled_pixel;

	scaled_pixel = SDL_MapRGB(screen->format, r, g, b);

	return scaled_pixel;
}

unsigned int GFX_Tint_Pixel(unsigned int pixel, TINT tint)
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	unsigned int scaled_pixel;

	SDL_GetRGB(pixel, screen->format, &r, &g, &b);

	if(r != 255 && g != 0 && b != 255)
	{
		r = (unsigned char)(((float)r) * tint.r);
		g = (unsigned char)(((float)g) * tint.g);
		b = (unsigned char)(((float)b) * tint.b);
	}

	scaled_pixel = SDL_MapRGB(screen->format, r, g, b);

	return scaled_pixel;
}


void GFX_draw_sprite(VECTOR2 sprite_position, VECTOR2 sprite_size, float height)
{
	VECTOR2 transformed_pos;
	GFX_TEXTURE_PARAM texture;

	texture.id = 11;

	texture.parallax = 0;

	texture.u_offset = 0;
	texture.v_offset = 0;

	texture.u_scale = 1.;
	texture.v_scale = 1.;

	float sprite_height;

	transformed_pos = sub_v2(sprite_position, player->pos);
	transformed_pos = rot_v2(transformed_pos, player->facing);

	float transformed_height = height - player->pos_height;

	float xscale = main_camera->camera_parameters_x / transformed_pos.y;
	float yscale = main_camera->camera_parameters_y / transformed_pos.y;

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

	TINT tint;

	tint.r = 0.1;
	tint.g = 0.1;
	tint.b = 0.6;

	for(int x = c_screen_x0; x < c_screen_x1; x++)
	{
		for(int y = c_screen_y0; y < c_screen_y1; y++)
		{
			if(transformed_pos.y < get_z_buffer(main_camera, x, y))
			{
				GFX_set_pixel_from_texture_tint(	screen, texture, x, y,
													(int)((float)(x-screen_x0)/(float)(screen_x1-screen_x0) * 128),
													(int)((float)(y-screen_y0)/(float)(screen_y1-screen_y0) * 256),
													tint);
			}
		}
	}
}

float get_view_plane_pos_x(int ssx)
{
	float pos_x = ((float)(ssx - SCREEN_RES_X/2)/(float)(SCREEN_RES_X/2)) *  main_camera->hither_x;

	return pos_x;
}

float get_view_angle_from_ss(int ssx)
{
	float angle_x = atan(get_view_plane_pos_x(ssx) / main_camera->hither_z);

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

	clear_z_buffer(main_camera);

	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		y_undrawn_top[x] = 0;
		y_undrawn_bot[x] = SCREEN_RES_Y; 
	} 

	start_screen_x = 0;
	end_screen_x = SCREEN_RES_X - 1;

	current_sector_id = player->current_sector;

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

			TINT current_tint = current_sector->tint;

			//if(start_screen_x == end_screen_x) continue;

			VECTOR2 edge_v0 = get_vertex_at(current_edge->v_start);
			VECTOR2 edge_v1 = get_vertex_at(current_edge->v_end);

			//Transform current edges to player point of view (player at 0,0)
			transformed_pos_0 = sub_v2(edge_v0, player->pos);
			transformed_pos_1 = sub_v2(edge_v1, player->pos);
			
			transformed_pos_0 = rot_v2(transformed_pos_0, player->facing);
			transformed_pos_1 = rot_v2(transformed_pos_1, player->facing);

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
					VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
					VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);

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
					VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
					VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);
						
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
			float xscale0 = main_camera->camera_parameters_x / transformed_pos_0.y;
			float yscale0 = main_camera->camera_parameters_y / transformed_pos_0.y;

			float xscale1 = main_camera->camera_parameters_x / transformed_pos_1.y;
			float yscale1 = main_camera->camera_parameters_y / transformed_pos_1.y;

			//Transform to pixel locations (and project)

			float proj_x0 = transformed_pos_0.x * xscale0;
			float proj_x1 = transformed_pos_1.x * xscale1;

			int x0 = (int)(proj_x0) + SCREEN_RES_X/2;
			int x1 = (int)(proj_x1) + SCREEN_RES_X/2;

			//If outside screen, get out
			if(x0 >= x1 || x1 < start_screen_x || x0 > end_screen_x) continue;

			//Get relative ceil and floor heights
			float yceil = current_sector->ceiling_height - player->pos_height;
			float yfloor = current_sector->floor_height - player->pos_height;

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

				nyceil = neighbor_sector->ceiling_height - player->pos_height;
				nyfloor = neighbor_sector->floor_height - player->pos_height;
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
									current_sector->text_param_ceil, 
									current_tint);

				GFX_draw_visplane(	x, c_screen_y_floor, y_undrawn_bot[x],
									0, yfloor + 0.02, 
									current_sector->text_param_floor, 
									current_tint);

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
										current_edge->text_param, current_tint); 
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
										current_edge->text_param, current_tint); 
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
									current_edge->text_param, current_tint);
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
					GFX_TEXTURE_PARAM texture_parameters,
					TINT tint)
{
	int text_x = (int)((u0*((x1-screen_x)*z1) + u1*((screen_x-x0)*z0)) / ((x1-screen_x)*z1 + (screen_x-x0)*z0));

	float z = (z1 - z0) * ((((screen_x-x0)*z0)) / ((x1-screen_x)*z1 + (screen_x-x0)*z0)) + z0;

	for(int screen_y = top_visible; screen_y < bot_visible; screen_y++)
	{
		if(texture_parameters.parallax)
		{
			int offset = (player->facing)/(2.*PI) * SKYBOX_SIZE_X;
			GFX_set_pixel_from_texture(	screen,
										texture_parameters,
										screen_x, screen_y,
										screen_x + offset, screen_y);
		}
		else
		{
			int text_y = (float)(screen_y - top_invisible)/(float)(bot_invisible - top_invisible) * (TEXTURE_SIZE_Y);

			GFX_set_pixel_from_texture_depth_tint(	screen,
													texture_parameters,
													screen_x, screen_y,
													text_x, text_y, 
													z, tint);

			set_z_buffer(main_camera, screen_x, screen_y, z);
		}
	}	
}

void GFX_draw_sprite_wall (	VECTOR2 start_pos, VECTOR2 end_pos,
							float bot_height, float top_height,
							GFX_TEXTURE_PARAM texture_parameters,
							TINT tint)
{
	//Transform current edges to player point of view (player at 0,0)
	VECTOR2 transformed_pos_0 = sub_v2(start_pos, player->pos);
	VECTOR2 transformed_pos_1 = sub_v2(end_pos, player->pos);
	
	transformed_pos_0 = rot_v2(transformed_pos_0, player->facing);
	transformed_pos_1 = rot_v2(transformed_pos_1, player->facing);

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
			VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);

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
			VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);
				
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
	float xscale0 = main_camera->camera_parameters_x / transformed_pos_0.y;
	float yscale0 = main_camera->camera_parameters_y / transformed_pos_0.y;

	float xscale1 = main_camera->camera_parameters_x / transformed_pos_1.y;
	float yscale1 = main_camera->camera_parameters_y / transformed_pos_1.y;

	//Transform to pixel locations (and project)

	float proj_x0 = transformed_pos_0.x * xscale0;
	float proj_x1 = transformed_pos_1.x * xscale1;

	int x0 = (int)(proj_x0) + SCREEN_RES_X/2;
	int x1 = (int)(proj_x1) + SCREEN_RES_X/2;

	//If outside screen, get out
	if(x0 >= x1 || x1 < 0 || x0 > SCREEN_RES_X) return;

	//Get relative ceil and floor heights
	float yceil = top_height - player->pos_height;
	float yfloor = bot_height - player->pos_height;

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
						texture_parameters, tint);

	}
}

void GFX_draw_sprite_wall_db (	VECTOR2 start_pos, VECTOR2 end_pos,
								float bot_height, float top_height,
								GFX_TEXTURE_PARAM texture_parameters,
								TINT tint)
{
	GFX_draw_sprite_wall(start_pos, end_pos, bot_height, top_height, texture_parameters, tint);
	GFX_draw_sprite_wall(end_pos, start_pos, bot_height, top_height, texture_parameters, tint);
}

void GFX_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters,
						TINT tint)
{
	VECTOR2 world_space;
	VECTOR2 relative_space;
	int is_visible = 1;

	if(is_ceiling && visplane_height < 0.) is_visible = 0;
	if((!is_ceiling) && visplane_height > 0.) is_visible = 0;

	if(is_visible)
	{
		for(int screen_y = visible_top; screen_y < visible_bot; screen_y++)
		{
			if(texture_parameters.parallax)
			{
				int offset = (player->facing)/(2.*PI) * SKYBOX_SIZE_X;
				GFX_set_pixel_from_texture(	screen,
											texture_parameters,
											screen_x, screen_y,
											screen_x + offset, screen_y);
			}
			else
			{
				relative_space = convert_ss_to_rs(main_camera, point2(screen_x, screen_y), visplane_height);
				world_space = convert_rs_to_ws(relative_space);

				GFX_set_pixel_from_texture_depth_tint(	screen,
														texture_parameters,
														screen_x, screen_y,
														(int)(world_space.x * 128.), (int)(world_space.y * 128.),
														relative_space.y, tint);
			
			}
		}
	}
}

void GFX_draw_hand()
{	
	GFX_TEXTURE_PARAM hand_texture;

	hand_texture.id = HAND_TEX_ID;
	hand_texture.parallax = 0;
	hand_texture.u_offset = 0;
	hand_texture.v_offset = 0;
	hand_texture.u_scale = 1.;
	hand_texture.v_scale = 1.;

	TINT tint;

	tint = (loaded_level.sectors + player->current_sector)->tint;

	for(int x = 0; x < SCREEN_RES_X; x ++)
	{
		for(int y = 0; y < SCREEN_RES_Y; y ++)
		{
			GFX_set_pixel_from_texture_tint(	screen,
												hand_texture,
												x, y,
												x, y, 
												tint);
		}
	}	
}