
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

//#include "editor.h"

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

extern LEVEL loaded_level; 
extern SDL_Surface * screen;

extern float current_fps;
extern bool show_fps;

char buffer[128];

void move_cursor(VECTOR2 amount)
{
	editor_cursor = sum_v2(editor_cursor, amount);
	get_closest_vertex(editor_cursor, &closest_vector, &closest_vector_index, &closest_vector_distance);
}

void move_view(VECTOR2 amount)
{
	editor_center = sum_v2(editor_center, amount);
	move_cursor(amount);
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

	if(keystate[SDLK_UP])
	{
		move_view(scale_v2(vector2(0, 1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_DOWN])
	{
		move_view(scale_v2(vector2(0, -1), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_RIGHT])
	{
		move_view(scale_v2(vector2(1, 0), cursor_speed * ENGINE_delta_time()));
	}
	
	if(keystate[SDLK_LEFT])
	{
		move_view(scale_v2(vector2(-1, 0), cursor_speed * ENGINE_delta_time()));
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
	}
}

void EDITOR_Loop()
{
}
