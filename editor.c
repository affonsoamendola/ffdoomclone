
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

float editor_zoom = 3.;

extern LEVEL loaded_level; 
extern SDL_Surface * screen;

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

void EDITOR_Handle_Input()
{

}

void EDITOR_Loop()
{

}
