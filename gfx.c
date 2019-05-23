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
#include "3d.h"

#include "gfx.h"

extern SDL_Surface * screen;

extern float current_fps;

extern bool show_fps;
extern bool show_map;

extern LEVEL loaded_level;

extern PLAYER * player;

extern bool edit_mode;

char buffer[128];

char * default_font_location;

CAMERA * main_camera;

GFX_TEXTURE loaded_textures[TEX_ID_SIZE];

GFX_TEXTURE_PARAM default_texture;

GFX_TEXTURE seven_seg_font;
GFX_TEXTURE tiny_text_font;
GFX_TEXTURE ui_tex;

GFX_TEXTURE hand_tex;

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

void GFX_load_texture_at(char* location, GFX_TEXTURE * holder)
{
	SDL_Surface * tex_surface;

	tex_surface = IMG_Load(location);

	if(tex_surface == NULL)
	{
		printf("Could not load texture, Error: %s\n", SDL_GetError());
		exit(-1);
	}

	SDL_SetColorKey (tex_surface, SDL_TRUE, SDL_MapRGB(tex_surface->format, 255, 0, 255));

	holder->loaded = 1;
	holder->surface = tex_surface;

	holder->size_x = tex_surface->w;
	holder->size_y = tex_surface->h;
}

void GFX_load_texture(char* location, int tex_id)
{
	GFX_load_texture_at(location, loaded_textures + tex_id);
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

	GFX_load_texture_at("graphix/7seg.png", &seven_seg_font);
	GFX_load_texture_at("graphix/ui.png", &ui_tex);
	GFX_load_texture_at("graphix/tinytext.png", &tiny_text_font);
	GFX_load_texture_at("graphix/coffeehands.png", &hand_tex);

	default_texture.id = 0;
	default_texture.parallax = 0;

	default_texture.u_offset = 0;
	default_texture.v_offset = 0;

	default_texture.u_scale = 1.;
	default_texture.v_scale = 1.;

	for(int i = 0; i < TEX_ID_SIZE; i++)
	{
		loaded_textures[i].loaded = 0;
		loaded_textures[i].surface = NULL;
		loaded_textures[i].size_x = 0;
		loaded_textures[i].size_y = 0;
	}

	GFX_load_resource_list("graphix/default.rls");

	GFX_load_texture("graphix/terminator.png", 11);

	UI_Init();
}

void GFX_Tick()
{
	UI_Tick();
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

SDL_Surface * GFX_new_surface(int width, int height)
{
	SDL_Surface * surface;

	unsigned int rmask, gmask, bmask, amask;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	    rmask = 0xff000000;
	    gmask = 0x00ff0000;
	    bmask = 0x0000ff00;
	    amask = 0x000000ff;
	#else
	    rmask = 0x000000ff;
	    gmask = 0x0000ff00;
	    bmask = 0x00ff0000;
	    amask = 0xff000000;
	#endif

	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);

	if(surface == NULL)
	{
		printf("\nSDL_CreateRGBSurface failed, error: %s\n", SDL_GetError());
		exit(-1);
	}

	return surface;
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
	SDL_FillRect(screen, NULL, 0);
}

void GFX_Render()
{
	GFX_clear_screen();

	G3D_render_3d();

	if(edit_mode == 0)
		GFX_draw_ui();
	else
		GFX_draw_ui_edit();

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

void GFX_blit(SDL_Surface * src, SDL_Surface * dst, SDL_Rect src_rect, POINT2 dst_pos, TINT tint)
{
	for(int x = 0; x < src_rect.w; x++)
	{
		for(int y = 0; y < src_rect.h; y++)
		{
			unsigned int pixel;

			pixel = GFX_get_pixel(src, src_rect.x + x, src_rect.y + y);
			pixel = GFX_Tint_Pixel(pixel, tint);
			GFX_set_pixel(screen, dst_pos.x + x, dst_pos.y + y, pixel, 1);
		}
	}
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

void GFX_draw_7_segment(POINT2 position, int number, TINT tint)
{
	int value[3];
	bool to_draw[3];

	value[0] = get_number_in_decimal_slot(number, 2);
	value[1] = get_number_in_decimal_slot(number, 1);
	value[2] = get_number_in_decimal_slot(number, 0);

	to_draw[0] = 1;
	to_draw[1] = 1;
	to_draw[2] = 1;

	if(value[0] == 0)
		to_draw[0] = 0;
	else
		to_draw[0] = 1;

	if(value[1] == 0 && value[0] == 0)
		to_draw[1] = 0;
	else
		to_draw[1] = 1;

	to_draw[2] = 1;

	for(int i = 0; i < 3; i++)
	{
		if(to_draw[i])
		{
			SDL_Rect location = (SDL_Rect){12 * value[i],0,12,22};

			GFX_blit(seven_seg_font.surface, screen, location, sum_p2(position, point2(i * location.w, 0)), tint);
		}		
	}
}

void GFX_draw_tiny_char(POINT2 position, char character, TINT tint)
{
	SDL_Rect location;

	int character_column = 12;
	int character_line = 0;

	if(character == '!') {character_column = 26; character_line = 1;}
	if(character == '?') {character_column = 27; character_line = 1;}
	if(character == ':') {character_column = 10; character_line = 0;}
	if(character == '/') {character_column = 11; character_line = 0;}
	if(character == '-') {character_column = 12; character_line = 0;}
	if(character == '+') {character_column = 13; character_line = 0;}
	if(character == '.') {character_column = 14; character_line = 0;}
	if(character == ',') {character_column = 15; character_line = 0;}
	if(character >= (int)'0' && character <= (int)'9') {character_column = (int)character - (int)'0'; character_line = 0;}
	if(character >= (int)'a' && character <= (int)'z') {character_column = (int)character - (int)'a'; character_line = 1;}
	if(character >= (int)'A' && character <= (int)'Z') {character_column = (int)character - (int)'A'; character_line = 1;}

	location = (SDL_Rect){4 * character_column, 6 * character_line, 4, 6};
	
	GFX_blit(tiny_text_font.surface, screen, location, position, tint);
}

void GFX_draw_tiny_string(POINT2 position, char* string, TINT tint)
{
	for(int i = 0; i < 256; i++)
	{
		if(*(string + i) == '\0')
		{
			break;
		}
		
		if(*(string + i) != ' ')
			GFX_draw_tiny_char(point2(position.x + i * 4, position.y), *(string + i), tint);
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

	if(!(r == 255 && g == 0 && b == 255))
	{
		r = (unsigned char)(((float)r) * tint.r);
		g = (unsigned char)(((float)g) * tint.g);
		b = (unsigned char)(((float)b) * tint.b);
	}

	scaled_pixel = SDL_MapRGB(screen->format, r, g, b);

	return scaled_pixel;
}
