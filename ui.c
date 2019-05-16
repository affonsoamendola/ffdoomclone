#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "point2.h"
#include "vector2.h"
#include "console.h"
#include "world.h"
#include "gfx.h"
#include "player.h"
#include "input.h"

#include "ui.h"

#include "engine.h"

extern SDL_Surface * screen;
extern LEVEL loaded_level;
extern PLAYER * player;

float map_scale = 20.0f;

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

			last_v_vector = sub_v2(last_v_vector, player->pos);
			current_v_vector = sub_v2(current_v_vector, player->pos);

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

void GFX_draw_ui()
{	
	GFX_TEXTURE_PARAM ui_texture;

	ui_texture.id = UI_TEX_ID;
	ui_texture.parallax = 0;
	ui_texture.u_offset = 0;
	ui_texture.v_offset = 0;
	ui_texture.u_scale = 1.;
	ui_texture.v_scale = 1.;

	TINT tint;

	tint = (loaded_level.sectors + player->current_sector)->tint;

	for(int x = 0; x < SCREEN_RES_X; x ++)
	{
		for(int y = 200; y < SCREEN_RES_Y; y ++)
		{
			GFX_set_pixel_from_texture(	screen,
										ui_texture,
										x, y,
										x, y);
		}
	}	
}