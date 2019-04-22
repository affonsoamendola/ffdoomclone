#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "input.h"
#include "console.h"
#include "SDL.h"
#include "point2.h"
#include "math.h"
#include "world.h"
#include "vector2.h"
#include "utility.h"
#include "list.h"

#include "gfx.h"

#define TEXTURE_SIZE_X 128
#define TEXTURE_SIZE_Y 128

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

float hither_z = 1e-4f;
float hither_x;
float hither_y;

float yon_z = 10.0f;
float yon_x;
float yon_y;

float depth_lighting_max_distance = 6.0f;

float hfov = PI/4;
float vfov;

float camera_parameters_x;
float camera_parameters_y;

//float hfov = 0.73f * SCREEN_RES_X;
//float vfov = 0.2f * SCREEN_RES_Y;

SDL_Surface * wall;
SDL_Surface * skybox;

void GFX_load_texture_wall(char * location)
{
	wall = SDL_LoadBMP(location);

	SDL_LockSurface(wall);
}
void GFX_load_texture_skybox(char * location)
{
	skybox = SDL_LoadBMP(location);


	SDL_LockSurface(skybox);
}

void GFX_Init()
{
	hither_x = tan(hfov) * hither_z;
	hither_y = (hither_x * SCREEN_RES_Y)/SCREEN_RES_X;

	yon_x = tan(hfov) * yon_z;
	yon_y = yon_x/ASPECT_RATIO;

	vfov = atan(hither_y/hither_z);

	camera_parameters_x = ((hither_z * (float)SCREEN_RES_X/2) / hither_x);
	camera_parameters_y = ((hither_z * (float)SCREEN_RES_Y/2) / hither_y);

	GFX_load_font("8x8Font.fnt");

	GFX_load_texture_wall("dopefish.bmp");
	GFX_load_texture_skybox("skybox.bmp");	
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

void GFX_set_pixel(SDL_Surface *surface, int x, int y, unsigned int pixel)
{
    int bpp = surface->format->BytesPerPixel;

    unsigned char *p_base = (unsigned char *)surface->pixels + (y*PIXEL_SCALE)*surface->pitch + (x*PIXEL_SCALE)*bpp;
    unsigned char *p;

    for(int i = 0; i < PIXEL_SCALE; i ++)
    {
    	for(int j = 0; j < PIXEL_SCALE; j ++)
    	{

    		p = p_base + i * bpp + j * 320 * PIXEL_SCALE * bpp;

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
		        *(unsigned int *)p = pixel;
		        break;
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
		GFX_set_pixel(surface, x, y, pixel);
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
		GFX_set_pixel(surface, x, y, pixel);
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
		GFX_set_pixel(surface, x, y, pixel);
		current_error += slope;
		if(fabs(current_error) >= 0.5f)
		{
			y = y + (current_error >= 0) * 1 + (current_error < 0) * (-1);
			current_error -= 1.0f;
		}
	}
}

void GFX_draw_texture_vert_line(SDL_Surface *surface, 
								SDL_Surface *texture, 
								int screen_x, int screen_y1, int screen_y2, 
								float relative_x, int screen_space_wall_y1, int screen_space_wall_y2)
{
	//This is very confusing, I need to rewrite this, BADLY

	int temp;

	if(screen_y1 > screen_y2)
	{
		temp = screen_y1;
		screen_y1 = screen_y2;
		screen_y2 = temp;
	}

	for(int screen_y = screen_y1; screen_y <= screen_y2; screen_y++)
	{
		float relative_y = (float)(screen_y - screen_space_wall_y1) /
						   (float)(screen_space_wall_y2 - screen_space_wall_y1);

		int texture_space_y = (int)(relative_y * (TEXTURE_SIZE_Y - 1));

		GFX_set_pixel(surface, screen_x, screen_y, GFX_get_pixel(texture, 
																((TEXTURE_SIZE_X - 1) - relative_x * (TEXTURE_SIZE_X-1)), 
																texture_space_y));
	}
}

void GFX_set_pixel_from_texture(SDL_Surface *surface,
								SDL_Surface *texture,
								int screen_x, int screen_y,
								int text_x, int text_y)
{

	GFX_set_pixel(surface, screen_x, screen_y, GFX_get_pixel(	texture, 
																abs(text_x % TEXTURE_SIZE_X), 
																abs(text_y % TEXTURE_SIZE_Y)));
}

void GFX_fill_rectangle(POINT2 start, POINT2 end, unsigned int pixel)
{
	for(int i = start.x; i <= end.x; i++)
	{
		for(int j = start.y; j <= end.y; j++)
		{
			GFX_set_pixel(screen, i, j, pixel);
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

	if(show_map)
	{
		GFX_set_pixel(screen, SCREEN_RES_X/2, SCREEN_RES_Y/2, SDL_MapRGB(screen->format, 255, 0, 0));
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
				GFX_set_pixel(screen, position.x + column, position.y + line, pixel);
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

	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		y_undrawn_top[x] = 0;
		y_undrawn_bot[x] = SCREEN_RES_Y-1; 
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
			float t_u1 = (float)(TEXTURE_SIZE_X-1);

			//If completely behind player, continue from loop
			if(transformed_pos_0.y <= 0 && transformed_pos_1.y <= 0) 
			{
				continue;
			}
	
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

				t_u0 = (transformed_pos_0.x - ni_pos_0.x) * (TEXTURE_SIZE_X-1)/(ni_pos_1.x - ni_pos_0.x);
				t_u1 = (transformed_pos_1.x - ni_pos_0.x) * (TEXTURE_SIZE_X-1)/(ni_pos_1.x - ni_pos_0.x);			
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
			if(x0 >= x1 || x1 < start_screen_x || x0 > end_screen_x) 
			{
				continue;
			}	

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

			for(int x = x_begin; x <= x_end; x++)
			{
				float relative_x = (float)(x-x0)/(float)(x1-x0);

				int screen_y_ceil = relative_x * (y1ceiling - y0ceiling) + y0ceiling;
				int screen_y_floor = relative_x * (y1floor - y0floor) + y0floor;

				int c_screen_y_ceil = clamp_int(screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
				int c_screen_y_floor = clamp_int(screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

				int text_x = (int)((t_u0*((x1-x)*transformed_pos_1.y) + t_u1*((x-x0)*transformed_pos_0.y)) / ((x1-x)*transformed_pos_1.y + (x-x0)*transformed_pos_0.y));
	
				VECTOR2 world_space;

				for(int y = y_undrawn_top[x]; y < c_screen_y_ceil; y ++)
				{
					//world_space = convert_ss_to_ws(point2(x, y), yceil);
					//GFX_set_pixel_from_texture(screen, texture, x, y, (int)(world_space.x * 128.), (int)(world_space.y * 128.));
					float angle_x = get_view_angle_from_ss(x);

					angle_x = player_facing + angle_x;

					while(angle_x > 2*PI)
					{
						angle_x -= 2*PI;
					}

					while(angle_x < 0)
					{
						angle_x += 2*PI;
					}

					GFX_set_pixel_from_texture(	screen, skybox, x, y, 
												(int)(((angle_x)/(2*PI) * 128.)), 
												(int)(((float)y / 160.) * 128.));

				}

				for(int y = c_screen_y_floor; y < y_undrawn_bot[x]+1; y ++)
				{
					world_space = convert_ss_to_ws(point2(x, y), yfloor);
					
					GFX_set_pixel_from_texture(screen, wall, x, y, (int)(world_space.x * 128.), (int)(world_space.y * 128.));
				}
				
				if(current_edge->is_portal)
				{
					int n_screen_y_ceil = relative_x * (ny1ceiling - ny0ceiling) + ny0ceiling;
					int n_screen_y_floor = relative_x * (ny1floor - ny0floor) + ny0floor;

					int c_n_screen_y_ceil = clamp_int(n_screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
					int c_n_screen_y_floor = clamp_int(n_screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

					if(c_n_screen_y_ceil > c_screen_y_ceil)
					{
						for(int y = c_screen_y_ceil; y < c_n_screen_y_ceil; y ++)
						{
							int text_y = (float)(y - screen_y_ceil)/(float)(n_screen_y_ceil- screen_y_ceil) * (TEXTURE_SIZE_Y-1);
							GFX_set_pixel_from_texture(screen, wall, x, y, text_x, text_y);
						}
					}

					y_undrawn_top[x] = clamp_int(max_int(c_screen_y_ceil, c_n_screen_y_ceil), SCREEN_RES_Y-1, y_undrawn_top[x]);

					if(c_n_screen_y_floor < c_screen_y_floor)
					{
						for(int y = c_n_screen_y_floor; y < c_screen_y_floor; y ++)
						{
							int text_y = (float)(y - n_screen_y_floor)/(float)(screen_y_floor - n_screen_y_floor) * (TEXTURE_SIZE_Y-1);
							GFX_set_pixel_from_texture(screen, wall, x, y, text_x, text_y);
						}
					}

					y_undrawn_bot[x] = clamp_int(min_int(c_screen_y_floor, c_n_screen_y_floor), y_undrawn_bot[x], 0);
				}
				else
				{
					for(int y = c_screen_y_ceil; y < c_screen_y_floor; y ++)
					{
						int text_y = (float)(y - screen_y_floor)/(float)(screen_y_ceil - screen_y_floor) * (TEXTURE_SIZE_Y-1);
						GFX_set_pixel_from_texture(screen, wall, x, y, text_x, text_y);
					}
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