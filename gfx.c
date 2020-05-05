#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "engine.h"
#include "ff_point2.h"
#include "ff_rect.h"
/*
#include "input.h"
#include "console.h"

#include "math.h"
#include "world.h"
#include "vector2.h"
#include "utility.h"
#include "list.h"
#include "player.h"
#include "ui.h"
#include "3d.h"
*/
#include "gfx.h"

GFX gfx;

//Loads a texture into the GPU
Texture GFX_load_texture(const char* location)
{
	Texture texture;

	SDL_Texture* sdl_texture;
	SDL_Surface* sdl_surface;

	sdl_surface = IMG_Load(location);

	if(sdl_surface == NULL)
	{
		printf("Could not load texture file, Error: %s\n", SDL_GetError());
		exit(-1);
	}

	sdl_texture = SDL_CreateTextureFromSurface(gfx.renderer, sdl_surface);

	if(sdl_texture == NULL)
	{
		printf("Could not create SDL Texture, Error: %s\n", SDL_GetError());
		exit(-1);
	}

	texture.w = sdl_surface->w;
	texture.h = sdl_surface->h;

	SDL_FreeSurface(sdl_surface);

	SDL_SetTextureBlendMode(sdl_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(sdl_texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(sdl_texture, 255);

	texture.texture = sdl_texture;

	texture.blend_mode = SDL_BLENDMODE_BLEND;
	texture.modulation = COLOR_WHITE;

	return texture;
}

//Frees a texture from memory.
void GFX_free_texture(Texture texture)
{

	SDL_DestroyTexture(texture.texture);
}

//Reads a texture resource list file (.trl)
//Format is supposed to be easily editable, and consists in a ASCII readable text file
//first line is composed of the characters TRL followed by a blank space (' ') and an
//integer with the number of entries in the list.
//after which begins each entry, which is composed of an index unsigned int followed by a 
//two ints with the texture size and a string of the texture png file (relative to executable.) 
//(max of 128 characters on the string, can be easily increased by changing the buffer array size)
//EX:
//TRL 3
//0 graphix/tex0.png //index 0 at graphix/tex0.png
//1 graphix/tex1.png //index 1 at graphix/tex1.png
//2 tex1234.png
int GFX_load_texture_list(const char* location, TextureRegistry* texture_registry)
{
	FILE* texture_list_file;

	texture_list_file = fopen(location, "r");

	if(texture_list_file != NULL)
	{
		if(fgetc(texture_list_file) != 'T') return 1;
		if(fgetc(texture_list_file) != 'R') return 1;
		if(fgetc(texture_list_file) != 'L') return 1;
		if(fgetc(texture_list_file) != ' ') return 1;

		uint32_t texture_amount = 0;

		fscanf(texture_list_file, "%d", &texture_amount);	

		texture_registry->textures = (Texture*)malloc(texture_amount * sizeof(Texture));
		texture_registry->size = texture_amount;

		for(int i = 0; i < texture_amount; i++)
		{
			char buffer[128];
			
			uint32_t index;
			
			fscanf(texture_list_file, "%d %s", &index, buffer);
			
			*(texture_registry->textures + index) = GFX_load_texture(buffer);
		}

		fclose(texture_list_file);

		return 0;
	}
	else return 1;
}

//Frees texture list.
void GFX_free_texture_list(TextureRegistry* texture_registry)
{
	for(int i = 0; i < texture_registry->size; i++)
	{
		GFX_free_texture(*(texture_registry->textures + i));
	}

	free(texture_registry->textures);
}

//Overrides the color mod of the texture for a while.
//Should be followed by a GFX_texture_return_color_mod after done.
void GFX_texture_override_color_mod(Texture texture, const Color color)
{
	SDL_SetTextureColorMod(texture.texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture.texture, color.a);
}

void GFX_texture_return_color_mod(Texture texture)
{
	SDL_SetTextureColorMod(texture.texture, texture.modulation.r, texture.modulation.g, texture.modulation.b);
	SDL_SetTextureAlphaMod(texture.texture, texture.modulation.a);
}

/*
void set_z_buffer(CAMERA * camera, int x, int y, float value)
{
	camera->z_buffer[x + (y*engine.screen_res_x)] = value;
}

float get_z_buffer(CAMERA * camera, int x, int y)
{
	return camera->z_buffer[x + y*engine.screen_res_x];
}

void clear_z_buffer(CAMERA * camera)
{
	for(int x = 0; x < engine.screen_res_x; x++)
	{
		for(int y = 0; y < engine.screen_res_y; y++)
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

	initted_camera.z_buffer = (float *)malloc(sizeof(float) * engine.screen_res_x * engine.screen_res_y);
	clear_z_buffer(&initted_camera);

	initted_camera.hither_x = tan(initted_camera.hfov) * initted_camera.hither_z;
	initted_camera.hither_y = (initted_camera.hither_x * engine.screen_res_y)/engine.screen_res_x;

	initted_camera.yon_x = tan(initted_camera.hfov) * initted_camera.yon_z;
	initted_camera.yon_y = initted_camera.yon_x/engine.aspect_ratio;

	initted_camera.vfov = atan(initted_camera.hither_y/initted_camera.hither_z);

	initted_camera.camera_parameters_x = ((initted_camera.hither_z * (float)engine.screen_res_x/2) / initted_camera.hither_x);
	initted_camera.camera_parameters_y = ((initted_camera.hither_z * (float)engine.screen_res_y/2) / initted_camera.hither_y);

	*camera = malloc(sizeof(CAMERA));
	**camera = initted_camera;
}

void GFX_Destroy_Camera(CAMERA * camera)
{
	free(camera->z_buffer);
	free(camera);
}
*/

//Initializes graphics subsystem
GFX* GFX_init()
{	
	//Initializes default values
	gfx.screen_res_x = 320;
	gfx.screen_res_y = 240;
	
	gfx.aspect_ratio = (float)gfx.screen_res_x/(float)gfx.screen_res_y;
	gfx.pixel_scale = 3;

	gfx.window_title = "Foffonso's Doomclone (Name pending)";

	//Creates SDL window
	gfx.window = SDL_CreateWindow(		gfx.window_title,                // window title
								        SDL_WINDOWPOS_UNDEFINED,         // initial x position
								        SDL_WINDOWPOS_UNDEFINED,         // initial y position
								        gfx.screen_res_x * gfx.pixel_scale,  // width, in pixels
								        gfx.screen_res_y * gfx.pixel_scale,  // height, in pixels
								        SDL_WINDOW_OPENGL                // flags
								    );


	if(gfx.window == NULL)
	{
		printf("Could not set up SDL2 window: %s\n", SDL_GetError());
		exit(1);
	}

	//Creates SDL renderer context.
	gfx.renderer = SDL_CreateRenderer	( 	gfx.window,
											-1,
											SDL_RENDERER_ACCELERATED
										);

	if(gfx.renderer == NULL)
	{
		printf("Problem creating SDL Renderer: %s\n", SDL_GetError());
		exit(1);
	}

	//Allocates the area that will be storing all the pixel information
	//Each pixel is in the RGBA8888 format, so 4 bytes per pixel
	gfx.screen_pixels = malloc(	gfx.screen_res_x * 
								gfx.screen_res_y * sizeof(Color));

	//Creates the texture that will be receiving all pixels
	//And will be flushed to screen after integer scaling.
	gfx.screen_surface = SDL_CreateTexture(	gfx.renderer,
											SDL_PIXELFORMAT_ABGR8888,
											SDL_TEXTUREACCESS_STREAMING,
											gfx.screen_res_x, 
											gfx.screen_res_y
										);

	SDL_SetTextureBlendMode(gfx.screen_surface, SDL_BLENDMODE_BLEND);
	/*	SDL_LockTexture(	gfx.screen_surface,
						NULL, 
						&gfx.screen_pixels, 
						&gfx.screen_pitch);*/

	//Sets texture to have nearest neighbor scaling
	//Which is the same as integer scaling when dealing with integer scaling factors.
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	if(GFX_load_font_list("graphix/default_font_list.frl", &gfx.font_registry) != 0)
	{
		printf("Problem loading default font list at: graphix/default_font_list.frl\n");
		exit(1);
	}

	if(GFX_load_texture_list("graphix/default_texture_list.trl", &gfx.texture_registry) != 0)
	{
		printf("Problem loading default texture list at: graphix/default_texture_list.frl\n");
		exit(1);
	}

	/*
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
	*/
}

void GFX_quit()
{
	//GFX_Destroy_Camera(main_camera);
	GFX_free_texture_list(&gfx.texture_registry);
	GFX_free_font_list(&gfx.font_registry);

	free(gfx.screen_pixels);
	SDL_DestroyTexture(gfx.screen_surface);
	SDL_DestroyRenderer(gfx.renderer);
	SDL_DestroyWindow(gfx.window);
}
/*
void GFX_Tick()
{
	UI_Tick();
}
*/
Color GFX_get_pixel(const int x, const int y)
{

   	return *(Color *)(gfx.screen_pixels + y*gfx.screen_res_x + x);
}

void GFX_set_pixel(	const int x, const int y, 
					const Color color)
{
	if(x >= 0 && x < gfx.screen_res_x && y >= 0 && y < gfx.screen_res_y)
	{
		memcpy(gfx.screen_pixels + (y*gfx.screen_res_x + x), &color, 4);
	}
}

void GFX_draw_vert_line(const int x, const int y_0, const int y_1, const Color color)
{
	for(int y = y_0; y <= y_1; y++)
	{
		GFX_set_pixel(x, y, color);
	}
}

void GFX_draw_hor_line(const int x_0, const int x_1, const int y, const Color color)
{
	for(int x = x_0; x <= x_1; x++)
	{
		GFX_set_pixel(x, y, color);
	}
}

void GFX_draw_line(Point2 p_0, Point2 p_1, const Color color)
{
	float slope;

	Point2 temp;

	float current_error = 0.0f;

	float x, y;

	if(p_0.x == p_1.x)
	{
		if(p_0.y > p_1.y)
		{
			GFX_draw_vert_line(p_0.x, p_1.y, p_0.y, color);
		}
		else
		{
			GFX_draw_vert_line(p_0.x, p_0.y, p_1.y, color);
		}
	}
	else if(p_0.y == p_1.y)
	{
		if(p_0.x > p_1.x)
		{
			GFX_draw_hor_line(p_1.x, p_0.x, p_0.y, color);
		}
		else
		{
			GFX_draw_hor_line(p_0.x, p_1.x, p_0.y, color);
		}
	}
	else if(abs(p_1.y - p_0.y) < abs(p_1.x - p_0.x))
	{
		if(p_0.x > p_1.x)
		{
			swap_p2(&p_0, &p_1);
		}

		y = p_0.y;
		
		slope = (float)(p_1.y - p_0.y)/(float)(p_1.x - p_0.x);

		for(int x = p_0.x; x <= p_1.x; x++)
		{
			GFX_set_pixel(x, y, color);

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
		if(p_0.y > p_1.y)
		{
			swap_p2(&p_0, &p_1);
		}

		x = p_0.x;
		
		slope = (float)(p_1.x - p_0.x)/(float)(p_1.y - p_0.y);

		for(int y = p_0.y; y <= p_1.y; y++)
		{
			GFX_set_pixel(x, y, color);

			current_error += slope;

			if(current_error >= 0.5f)
			{
				x = x + 1;
				current_error -= 1.0f;
			}
			else if(current_error <= -0.5f)
			{
				x = x - 1;
				current_error += 1.0f;
			}
		}
	}
}
/*
unsigned int GFX_get_pixel_from_texture(	GFX_TEXTURE_PARAM texture,
											float text_x, int text_y)
{
	int u;
	int v;

	int text_size_x = loaded_textures[texture.id].size_x;
	int text_size_y = loaded_textures[texture.id].size_y;

	u = (int)((float)text_x / texture.u_scale) - texture.u_offset;
	v = (int)((float)text_y / texture.v_scale) - texture.v_offset;
	
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
										float text_x, int text_y, float depth)
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
*/
void GFX_fill_rectangle(const Rect rect, const Color color)
{
	for(int y = rect.y; y < rect.y + rect.h; y++)
	{
		GFX_draw_hor_line(rect.x, rect.x + rect.w - 1, y, color);
	}
}

void GFX_fill_screen(const Color color)
{
	for(int x = 0; x < gfx.screen_res_x; x++)
	{
		for(int y = 0; y < gfx.screen_res_y; y++)
		{
			memcpy(gfx.screen_pixels + y*gfx.screen_res_x + x, &color, 4);
		}
	}
}

void GFX_clear_screen()
{

	memset(gfx.screen_pixels, 0, gfx.screen_res_x * gfx.screen_res_y*4);
}

void GFX_render_start()
{
	//Clears everything
	//Renderer
    SDL_SetRenderDrawColor(gfx.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(gfx.renderer);

	//Pixel Buffer.
	GFX_clear_screen();
}

void GFX_render_end()
{
	//Shows on screen
    SDL_RenderPresent(gfx.renderer);

    //printf("AVG FPS= %f FPS= %f DELTA_TIME= %f\n", engine_average_fps(), engine_fps(), engine_delta_time());
}

void GFX_update_pixels()
{
	//Updates screen texture with pixel information.
	SDL_UpdateTexture(
	    gfx.screen_surface,
	    NULL,
	   	gfx.screen_pixels,
	    gfx.screen_res_x*4
	    );

    //Send texture to rendering context (Does scaling automagically.)
	SDL_RenderCopy(gfx.renderer, gfx.screen_surface, NULL, NULL);
}

//Copies image from src to dst, doesnt apply scaling.
//Src Rect is the rect of the image to be blitted, src_size needs to be set to the size
//Of the ENTIRE image (src_size.x is different than src_rect.w if the src image is a spritesheet for example)
void GFX_blit(	Color* src, Color* dst, 
				const Rect src_rect , const uint32_t src_pitch, 
				const Point2 dst_pos, const uint32_t dst_pitch)
{
	if(dst == NULL)
	{

		dst = gfx.screen_pixels;
	}

	for(int y = 0; y < src_rect.h; y++)
	{
		memcpy(	dst + dst_pos.x + (y + dst_pos.y)*dst_pitch, 
				src + src_rect.x + (y + src_rect.y)*src_pitch,
				src_rect.w*4);
	}
}

//Performs an alpha blending blit of the source pixel array to the destination
//Pixel array.
//VERY SLOW, USE TEXTURES AND THE RENDERER INSTEAD. (GFX_blit_alpha_texture)
void GFX_blit_alpha(	Color* p_src, Color* p_dst, 
						const Rect src_rect, const uint32_t src_pitch, 
						const Point2 dst_pos, const uint32_t dst_pitch)
{
	if(p_dst == NULL)
	{

		p_dst = gfx.screen_pixels;
	}

	for(int x = 0; x < src_rect.w; x++)
	{
		for(int y = 0; y < src_rect.h; y++)
		{
			Color result;
			Color src = *(p_src + x + src_rect.x + (y + src_rect.y)*src_pitch);
			Color* dst_address = (p_dst + x + dst_pos.x  + (y + dst_pos.y )*dst_pitch);
			Color dst = *dst_address;

			result.r = ((int)src.r*src.a)/0xff + ((int)dst.r*(0xff - src.a))/0xff;
			result.g = ((int)src.g*src.a)/0xff + ((int)dst.g*(0xff - src.a))/0xff;
			result.b = ((int)src.b*src.a)/0xff + ((int)dst.b*(0xff - src.a))/0xff;
			result.a = src.a + ((int)dst.a * (0xff - src.a))/0xff;
			
			*dst_address = result;
		}
	}
}

//Performs a blit with some kind of blending function
//if dst is NULL, then the dst is set to the screen.
void GFX_blit_blend(	Color* src, Color* dst, 
						const Rect src_rect, const uint32_t src_pitch, 
						const Rect dst_rect, const uint32_t dst_pitch,
						const GFX_BLEND_MODE blend_mode, void* blend_data)
{
	switch(blend_mode)
	{
		case GFX_NO_BLEND:
			GFX_blit( 	src, dst,
						src_rect, src_pitch,
						point2(dst_rect.x, dst_rect.y), dst_pitch);
			break;
		case GFX_ALPHA_BLEND:
			GFX_blit_alpha( src, dst,
							src_rect, src_pitch,
							point2(dst_rect.x, dst_rect.y), dst_pitch);
			break;
	}
}

//Gets a texture pointer to a texture from the texture registry
Texture* GFX_get_texture_id(uint32_t texture_id)
{
	return gfx.texture_registry.textures + texture_id;
}

//Copies image from src to dst.
//Applies integer scaling if scale is != (1,1)
void GFX_blit_texture(	Texture* src, Texture* dst, 
						Rect src_rect, Point2 dst_offset,
						Point2 scale)
{
	//Creates the desination rect from the offset and texture sizes
	Rect dst_rect;

	//Explanantion below as to why I check for .h == 0
	if(src_rect.h == 0)
	{
		dst_rect = rect(dst_offset.x, dst_offset.y, src->w, src->h);
	}
	else
	{
		dst_rect = rect(dst_offset.x, dst_offset.y, src_rect.w, src_rect.h);
	}

	//Scales the rect by the engine pixel scale and the non-square scale argument
	Point2 pixel_scale = scale_i_p2(scale, gfx.pixel_scale);
	dst_rect = scalei_rect(dst_rect, pixel_scale.x, pixel_scale.y);

	if(dst == NULL)
	{
		//If .h is 0, then it is a GFX_BLIT_ALL rect.
		//Kind of a weird way of setting this, but you should never blit a 0 height texture.
		//So this makes a fast way of checking if you want to blit the entire image.
		if(src_rect.h == 0)
		{
			SDL_RenderCopy(	gfx.renderer,
							src->texture,
							NULL,
							(SDL_Rect*)&dst_rect);
		}
		else
		{
			SDL_RenderCopy(	gfx.renderer,
							src->texture,
							(SDL_Rect*)&src_rect,
							(SDL_Rect*)&dst_rect);
		}
	}
}

//Blits texture based on Texture ID
void GFX_blit_texture_id( 	uint32_t texture_id_src, Texture* dst,
							Rect src_rect, Point2 dst_offset,
							Point2 scale)
{
	GFX_blit_texture(	GFX_get_texture_id(texture_id_src), dst, 
						src_rect, dst_offset,
						scale);
}

//Reads a font resource list file (.frl)
//Format is supposed to be easily editable, and consists in a ASCII readable text file
//first line is composed of the characters FRL followed by a blank space (' ') and an
//integer with the number of entries in the list.
//after which begins each entry, which is composed of an index unsigned int followed by a 
//two ints with the font char size and a string of the font png file (relative to executable.) 
//(max of 128 characters on the string, can be easily increased by changing the buffer array size)
//EX:
//FRL 3
//0 8 8 graphix/font0.png //index 0 size 8x8 at graphix/font0.png
//1 4 6 graphix/font1.png //index 0 size 4x6 at graphix/font1.png
//2 16 16 font1234.png
int GFX_load_font_list(const char* location, FontRegistry* font_registry)
{
	FILE* font_list_file;

	font_list_file = fopen(location, "r");

	if(font_list_file != NULL)
	{
		if(fgetc(font_list_file) != 'F') return 1;
		if(fgetc(font_list_file) != 'R') return 1;
		if(fgetc(font_list_file) != 'L') return 1;
		if(fgetc(font_list_file) != ' ') return 1;

		uint32_t font_amount = 0;

		fscanf(font_list_file, "%d", &font_amount);

		font_registry->fonts = (Font*)malloc(font_amount * sizeof(Font));
		font_registry->size = font_amount;

		for(int i = 0; i < font_amount; i++)
		{
			char buffer[128];
			
			uint32_t index;
			uint32_t size_x;
			uint32_t size_y;
			
			fscanf(font_list_file, "%d %d %d %s", &index, &size_x, &size_y, buffer);
			
			*(font_registry->fonts + index) = GFX_load_font(buffer, size_x, size_y);
		}

		fclose(font_list_file);

		return 0;
	}
	else return 1;
}

//Frees a font list
void GFX_free_font_list(FontRegistry* font_registry)
{
	for(int i = 0; i < font_registry->size; i++)
	{
		GFX_free_font(*(font_registry->fonts + i));
	}

	free(font_registry->fonts);
}

//Transforms font_id into font object.
Font GFX_get_font(uint32_t font_id)
{

	return gfx.font_registry.fonts[font_id];
}

//Loads a font of size x and y
Font GFX_load_font(const char* location, uint32_t size_x, uint32_t size_y)
{
	Font font;

	font.char_w = size_x;
	font.char_h = size_y;

	font.texture = GFX_load_texture(location);

	return font;
}

//Frees a font
void GFX_free_font(Font font)
{

	GFX_free_texture(font.texture);
}

//Draws a char on screen
void GFX_draw_char_font(const Point2 position, Font font, char character)
{
	char char_index = character - 32;
	
	Rect char_rect;

	char_rect.x = (char_index%32) * font.char_w;
	char_rect.y = (char_index/32) * font.char_h;
	char_rect.w = font.char_w;
	char_rect.h = font.char_h;

	GFX_blit_texture(	&font.texture, NULL, 
						char_rect, position,
						GFX_NO_SCALE);
}

//Draws a char on screen from a font_id number
void GFX_draw_char(const Point2 position, uint32_t font_id, char character)
{

	GFX_draw_char_font(position, GFX_get_font(font_id), character);
}

//Applies a color modulation to the character.
//Overriding the default color modulation of the font sprite sheet.
void GFX_draw_char_color(const Point2 position, uint32_t font_id, char character, Color color)
{
	Font font = GFX_get_font(font_id);
	GFX_texture_override_color_mod(font.texture, color);
	GFX_draw_char_font(position, font, character);
	GFX_texture_return_color_mod(font.texture);
}

void GFX_draw_string(const Point2 position, uint32_t font_id, char* string)
{
	Font font = GFX_get_font(font_id);

	uint32_t i = 0;
	char current_char = *string;
	while(current_char != '\0')
	{
		GFX_draw_char_font(	point2(position.x + i*font.char_w, position.y), 
							font, current_char);
		i++;
		current_char = *(string + i);
	}
}

void GFX_draw_string_color(const Point2 position, uint32_t font_id, char* string, Color color)
{
	Font font = GFX_get_font(font_id);
	GFX_texture_override_color_mod(font.texture, color);
	GFX_draw_string(position, font_id, string);
	GFX_texture_return_color_mod(font.texture);
}

void GFX_draw_string_f(const Point2 position, uint32_t font_id, char* format_string, ...)
{
	char buffer[256] = {0};
	
	va_list args;

	va_start(args, format_string);
	vsnprintf(buffer, 256, format_string, args);
	va_end(args);

	GFX_draw_string(position, font_id, buffer);
}

void GFX_draw_string_color_f(const Point2 position, uint32_t font_id, Color color, char* format_string, ...)
{
	char buffer[256] = {0};
	
	va_list args;

	va_start(args, format_string);
	vsnprintf(buffer, 256, format_string, args);
	va_end(args);

	GFX_draw_string_color(position, font_id, buffer, color);
}
/*
void GFX_draw_7_segment(const Point2 position, unsigned int number, Color color)
{

}*/