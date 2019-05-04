
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "editor.h"

#include "vector2.h"
#include "world.h"
#include "gfx.h"
#include "engine.h"

VECTOR2 editor_center;
VECTOR2 editor_cursor;

int closest_vector_index;
VECTOR2 closest_vector;
float closest_vector_distance;

float grid_size = 0.1;

float editor_zoom = 50.;

float cursor_speed = 1.f;
#define walk_cursor_speed 3.f;
#define run_cursor_speed 6.f;

#define cursor_color GFX_Color(0, 0, 255)

extern LEVEL loaded_level; 
extern SDL_Surface * screen;

extern float current_fps;
extern bool show_fps;

int grabbed = 0;

int grabbed_vector_index;
VECTOR2 grabbed_vector;

char buffer[128];

VECTOR2 top_left_border;
VECTOR2 bottom_right_border;

void move_cursor(VECTOR2 amount)
{
	editor_cursor = sum_v2(editor_cursor, amount);
	get_closest_vertex(editor_cursor, &closest_vector, &closest_vector_index, &closest_vector_distance);

	if(	editor_cursor.x < top_left_border.x || editor_cursor.y >= top_left_border.y ||
		editor_cursor.x >= bottom_right_border.x || editor_cursor.y < bottom_right_border.y)
	{
		move_view(amount);
	}
}

void move_view(VECTOR2 amount)
{
	editor_center = sum_v2(editor_center, amount);

	top_left_border = convert_editor_ss_to_ws(point2(0, 0));
	bottom_right_border = convert_editor_ss_to_ws(point2(SCREEN_RES_X, SCREEN_RES_Y));
}

void create_vertex_at_cursor()
{
}

VECTOR2 get_closest_grid(VECTOR2 pos)
{
	int x;
	int y;

	VECTOR2 closest_grid;

	x = round(pos.x/grid_size);
	y = round(pos.y/grid_size);

	closest_grid.x = (float)x * grid_size;
	closest_grid.y = (float)y * grid_size;
}

void set_grid_size(float size)
{
	grid_size = size;
}

void set_zoom(float zoom)
{
	editor_zoom = zoom;
}

POINT2 convert_ws_to_editor_ss(VECTOR2 pos)
{
	POINT2 editor_ss;

	editor_ss.x = (pos.x - editor_center.x)*editor_zoom + SCREEN_RES_X/2;
	editor_ss.y = SCREEN_RES_Y/2 - (pos.y - editor_center.y)*editor_zoom;

	return editor_ss;
}

VECTOR2 convert_editor_ss_to_ws(POINT2 pos)
{
	VECTOR2 ws;

	ws.x = (pos.x - SCREEN_RES_X/2)/editor_zoom + editor_center.x;
	ws.y = -((pos.y - SCREEN_RES_Y/2)/editor_zoom - editor_center.y);

	return ws;
}

void draw_cursor()
{
	POINT2 cursor_ss;

	cursor_ss = convert_ws_to_editor_ss(editor_cursor);

	GFX_set_pixel(screen, cursor_ss.x, cursor_ss.y + 3, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x, cursor_ss.y + 2, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x, cursor_ss.y - 2, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x, cursor_ss.y - 3, GFX_Map_Color(cursor_color), 0);

	GFX_set_pixel(screen, cursor_ss.x + 3, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x + 2, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x - 2, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(screen, cursor_ss.x - 3, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
}

void draw_editor_map()
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

			current_line_start = convert_ws_to_editor_ss(last_v_vector);
			current_line_end = convert_ws_to_editor_ss(current_v_vector);

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

void EDITOR_Render()
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

	draw_editor_map();
	draw_cursor();

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

void EDITOR_enter_click()
{
}

void grab_vertex()
{
	grabbed = 1;
	
	grabbed_vector_index = closest_vector_index;
	grabbed_vector = closest_vector;
}

void drop_vertex()
{
	grabbed = 0;

	loaded_level.vertexes[grabbed_vector_index] = editor_cursor;
}

extern bool e_running;
SDL_Event event;

void EDITOR_Handle_Input()
{
	unsigned char * keystate = SDL_GetKeyState(NULL); 

	if(keystate[SDLK_LSHIFT])
	{
		cursor_speed = run_cursor_speed;
	}
	else
	{
		cursor_speed = walk_cursor_speed;
	}

	if(keystate[SDLK_w])
	{
		move_view(scale_v2(vector2(0, 1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_s])
	{
		move_view(scale_v2(vector2(0, -1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_d])
	{
		move_view(scale_v2(vector2(1, 0), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_a])
	{
		move_view(scale_v2(vector2(-1, 0), cursor_speed * ENGINE_delta_time()));
	}

	if(keystate[SDLK_UP])
	{
		move_cursor(scale_v2(vector2(0, 1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_DOWN])
	{
		move_cursor(scale_v2(vector2(0, -1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_RIGHT])
	{
		move_cursor(scale_v2(vector2(1, 0), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_LEFT])
	{
		move_cursor(scale_v2(vector2(-1, 0), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_PAGEUP])
	{
		editor_zoom *= 1.0f + 2.0f * ENGINE_delta_time();
	}
	
	if(keystate[SDLK_PAGEDOWN])
	{
		editor_zoom *= 1.0f - 2.0f * ENGINE_delta_time();
	}

	while(SDL_PollEvent(&event) != 0)
	{
		if(event.type == SDL_QUIT)
		{
			e_running = false;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == 'g')
			{
				if(grabbed == 0)
					grab_vertex();
				else if(grabbed == 1)
					drop_vertex();
			}
		}
	}
}

void EDITOR_Loop()
{
	if(grabbed == 1)
	{
		loaded_level.vertexes[grabbed_vector_index] = editor_cursor;
	}
}

void EDITOR_Init()
{
	editor_center = vector2(0., 0.);
	editor_cursor = vector2(0., 0.);
}
