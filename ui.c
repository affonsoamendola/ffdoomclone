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
#include "editor.h"

#include "face.h"

#include "ui.h"

#include "engine.h"

extern SDL_Surface * screen;
extern LEVEL loaded_level;
extern PLAYER * player;

float map_scale = 20.0f;

char buffer[128];

extern bool show_map;
extern bool show_fps;

extern float current_fps;

extern bool edit_mode;

extern GFX_TEXTURE ui_tex;
extern GFX_TEXTURE hand_tex;

extern int show_texture_select;
extern int selecting_texture_for;

extern bool show_help;

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

void GFX_draw_ammo_line(POINT2 location, int weapon_number)
{
	TINT color;

	if(player->current_weapon == weapon_number)
		color = GFX_Tint(1., 0.8, 0.6);
	else
		color = GFX_Tint(0.6, 0.3, 0.1);

	sprintf(buffer, "%i", weapon_number);
	GFX_draw_tiny_string(location, buffer, color);

	GFX_draw_tiny_char(sum_p2(location, point2(3, 0)), ':', color);
	
	sprintf(buffer, "%i", player->ammo[weapon_number]);

	int offset = 0;

	if(player->max_ammo[weapon_number] < 100)
		offset = -4;

	if(player->ammo[weapon_number] >= 100)
		GFX_draw_tiny_string(sum_p2(location, point2(7 + offset, 0)), buffer, color);
	else if(player->ammo[weapon_number] >= 10)
		GFX_draw_tiny_string(sum_p2(location, point2(11 + offset, 0)), buffer, color);
	else
		GFX_draw_tiny_string(sum_p2(location, point2(15 + offset, 0)), buffer, color);

	GFX_draw_tiny_char(sum_p2(location, point2(20 + offset, 0)), '/', color);

	sprintf(buffer, "%i", player->max_ammo[weapon_number]);
	GFX_draw_tiny_string(sum_p2(location, point2(24 + offset, 0)), buffer, color);
}

void GFX_draw_ammo_inventory()
{
	GFX_draw_ammo_line(point2(198,218), 2);
	GFX_draw_ammo_line(point2(198,224), 3);
	GFX_draw_ammo_line(point2(198,230), 4);

	GFX_draw_ammo_line(point2(236,218), 5);
	GFX_draw_ammo_line(point2(236,224), 6);
	GFX_draw_ammo_line(point2(236,230), 7);

	GFX_draw_ammo_line(point2(274,218), 8);
	GFX_draw_ammo_line(point2(274,224), 9);
	GFX_draw_ammo_line(point2(274,230), 0);
}

void GFX_draw_ui_bar()
{
	unsigned int pixel;

	for(int x = 0; x < SCREEN_RES_X; x ++)
	{
		for(int y = 0; y < 40; y ++)
		{
			pixel = GFX_get_pixel(ui_tex.surface, x, y);
			GFX_set_pixel(screen, x, y + SCREEN_RES_Y - 40, pixel, 1);
		}
	}	

	if(player->health >= 100)
		GFX_draw_7_segment(point2(2, 214), player->health, GFX_Tint(1., 1., 1.));
	else
		GFX_draw_7_segment(point2(0, 214), player->health, GFX_Tint(1., 1., 1.));

	if(player->armor >= 100)
		GFX_draw_7_segment(point2(38, 214), player->armor, GFX_Tint(1., 1., 1.));
	else
		GFX_draw_7_segment(point2(36, 214), player->armor, GFX_Tint(1., 1., 1.));

	GFX_draw_ammo_inventory();
	FACE_Draw();
}

void GFX_draw_help()
{
	int location_x = 180;

	GFX_draw_tiny_string(point2(location_x, 80), "R : Select ceiling.", GFX_Tint(1.0, 0.3, 0.0));
	GFX_draw_tiny_string(point2(location_x, 86), "T : Select closest wall.", GFX_Tint(1.0, 0.3, 0.0));
	GFX_draw_tiny_string(point2(location_x, 92), "Y : Select floor.", GFX_Tint(1.0, 0.3, 0.0));
	GFX_draw_tiny_string(point2(location_x, 98), "G : Open texture selector.", GFX_Tint(1.0, 0.3, 0.0));

	GFX_draw_tiny_string(point2(location_x, 106), "Home : Increase selected height.", GFX_Tint(1.0, 0.3, 0.0));
	GFX_draw_tiny_string(point2(location_x, 112), "End : Decrease selected height.", GFX_Tint(1.0, 0.3, 0.0));
}

void GFX_draw_ui_edit()
{
	if(show_texture_select)
	{
		EDITOR_draw_texture_select();
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

	sprintf(buffer, "Player X : %f", player->pos.x);
	GFX_draw_tiny_string(point2(0, 220), buffer, GFX_Tint(1.0, 0.3, 0.0));
	sprintf(buffer, "Player Y : %f", player->pos.y);
	GFX_draw_tiny_string(point2(0, 226), buffer, GFX_Tint(1.0, 0.3, 0.0));

	GFX_TEXTURE_PARAM selected_texture_param;

	if(selecting_texture_for == 0)
	{
		sprintf(buffer, "Ceiling selected");
		selected_texture_param = player->closest_sector->text_param_ceil;
	}
	else if(selecting_texture_for == 1)
	{
		sprintf(buffer, "Floor selected");
		selected_texture_param = player->closest_sector->text_param_floor;
	}
	else if(selecting_texture_for == 2)
	{
		sprintf(buffer, "Closest Wall selected");
		selected_texture_param = player->closest_edge->text_param;
	}

	GFX_draw_string(point2(0, 232), buffer, SDL_MapRGB(screen->format, 255, 200, 0));

	sprintf(buffer, "Offset U : %i V : %i", selected_texture_param.u_offset, selected_texture_param.v_offset);
	GFX_draw_tiny_string(point2(0, 200), buffer, GFX_Tint(1.0, 0.3, 0.0));
	sprintf(buffer, "Scale U : %f V : %f", selected_texture_param.u_scale, selected_texture_param.v_scale);
	GFX_draw_tiny_string(point2(0, 206), buffer, GFX_Tint(1.0, 0.3, 0.0));
	sprintf(buffer, "Parallax : %i", selected_texture_param.parallax);
	GFX_draw_tiny_string(point2(0, 212), buffer, GFX_Tint(1.0, 0.3, 0.0));

	sprintf(buffer, "Current Sector: %i", player->closest_sector->sector_id);
	GFX_draw_tiny_string(point2(0, 192), buffer, GFX_Tint(1.0, 0.3, 0.0));

	sprintf(buffer, "Current Ceil Height: %f", player->closest_sector->ceiling_height);
	GFX_draw_tiny_string(point2(0, 186), buffer, GFX_Tint(1.0, 0.3, 0.0));

	sprintf(buffer, "Current Floor Height: %f", player->closest_sector->floor_height);
	GFX_draw_tiny_string(point2(0, 180), buffer, GFX_Tint(1.0, 0.3, 0.0));

	if(show_help)
	{
		GFX_draw_help();
	}
}

void GFX_draw_ui()
{	
	GFX_draw_hand();
	GFX_draw_ui_bar();

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
}

void GFX_draw_hand()
{	
	TINT tint;
	tint = (loaded_level.sectors + player->current_sector)->tint;

	GFX_blit(hand_tex.surface, screen, SCREEN_RECT, ZERO_POINT2, tint);
}

void UI_Init()
{
	FACE_Init();
}

void UI_Tick()
{
	FACE_Tick();
}