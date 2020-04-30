
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "world.h"
#include "console.h"
#include "vector3.h"
#include "player.h"

char buffer[128];

extern LEVEL loaded_level;

extern PLAYER * player;

void COMMAND_ver()
{
	CONSOLE_print("\nRT76800");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
}

void COMMAND_intro()
{
	CONSOLE_print("\n----------------------------------------");
	CONSOLE_print("\nFofonso's DoomClone Engine\n");
	CONSOLE_print("\nCodename DN F3D");
	CONSOLE_print("\nCopyright Affonso Amendola, 2019");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
	CONSOLE_print("\n\nBe Excellent to Each Other");
	CONSOLE_print("\n----------------------------------------");
}

void COMMAND_help(int page)
{
	if(page == 1)
	{
		CONSOLE_print("\n----------------------------------------");
		CONSOLE_print("\nCommand List Page 1/3");
		CONSOLE_print("\nhelp [page]:");
		CONSOLE_print("\n  shows command list's specified page");
		CONSOLE_print("\nver: shows current game version");
		CONSOLE_print("\nintro: shows console intro again");
		CONSOLE_print("\nsector show [sector value]:");
		CONSOLE_print("\n  shows the sector's vertexes info");
		CONSOLE_print("\nnoclip: enables no-clip mode");
		CONSOLE_print("\n----------------------------------------");
	}
	else
	if(page == 2)
	{
		CONSOLE_print("\n----------------------------------------");
		CONSOLE_print("\nCommand List Page 2/3");
		CONSOLE_print("\nvertex list: ");
		CONSOLE_print("\n  list all vertexes in the current level");
		CONSOLE_print("\nset tint [r] [g] [b]:");
		CONSOLE_print("\n  sets the tint value of the player's   ");
		CONSOLE_print("\n  sector to the [r] [g] [b] values      ");
		CONSOLE_print("\n  specified, from 0 to 1");
		CONSOLE_print("\nset health [value]: sets health to value");
		CONSOLE_print("\n----------------------------------------");
	}
	else
	if(page == 3)
	{
		CONSOLE_print("\n----------------------------------------");
		CONSOLE_print("\nCommand List Page 3/3");
		CONSOLE_print("\nset armor [value]: sets armor to value");
		CONSOLE_print("\nedit: enables edit mode");
		CONSOLE_print("\nsave [filename]: ");
		CONSOLE_print("\n  saves current edited level as filename");
		CONSOLE_print("\nload [filename]: ");
		CONSOLE_print("\n  loads level from filename");
		CONSOLE_print("\n");
		CONSOLE_print("\n----------------------------------------");
	}
	
}

void COMMAND_sector_show(int sector_index)
{
	if(sector_index >= 0 && sector_index < loaded_level.s_num)
	{
		CONSOLE_print("\nx  y  is_portal neighbor_sector_id");

		for(int e = 0; e < (loaded_level.sectors+sector_index)->e_num; e++)
		{
			EDGE current_edge;

			current_edge = *(((loaded_level.sectors+sector_index)->e+e));

			sprintf(buffer, "\n%u  %u  %u         %u", current_edge.v_start, current_edge.v_end, current_edge.is_portal, current_edge.neighbor_sector_id);
			CONSOLE_print(buffer);
		}
	}
	CONSOLE_print("\n");
}

void COMMAND_vertex_list()
{
	for(int v = 0; v < loaded_level.v_num; v++)
	{
		sprintf(buffer, "\n%f %f", (loaded_level.vertexes + v)->x, (loaded_level.vertexes + v)->y);
		CONSOLE_print(buffer);
	}
}

void COMMAND_noclip()
{
	player->noclip = !(player->noclip);

	if(player->noclip == 1)
		CONSOLE_print("\nFLY YOU FOOL");

	if(player->noclip == 0)
		CONSOLE_print("\nYOU ARE GROUNDED");
}

void COMMAND_set(char * variable, int value)
{
	if(strcmp(variable, "health") == 0)
	{
		CONSOLE_print("\nSetting health to ");
		CONSOLE_printi(value);
		player->health = value;
	}

	if(strcmp(variable, "armor") == 0)
	{
		CONSOLE_print("\nSetting armor to ");
		CONSOLE_printi(value);
		player->armor = value;
	}
}

void COMMAND_set_tint(float r, float g, float b)
{
	player->closest_sector->tint = GFX_Tint(r, g, b);
}

void COMMAND_save_level(char * filename)
{
	CONSOLE_print("\nSaving...");
	FILE * new_file;

	new_file = fopen(filename, "w");

	sprintf(buffer, "%i\n", loaded_level.v_num);
	fputs(buffer, new_file);

	sprintf(buffer, "%i\n\n", loaded_level.s_num);
	fputs(buffer, new_file);

	for(int v = 0; v < loaded_level.v_num; v++)
	{
		sprintf(buffer, "%f %f\n", loaded_level.vertexes[v].x, loaded_level.vertexes[v].y);
		fputs(buffer, new_file);
	}

	fputs("\n", new_file);

	for(int s = 0; s < loaded_level.s_num; s++)
	{
		SECTOR * current_sector;

		current_sector = loaded_level.sectors + s;

		sprintf(buffer, "%u\n", current_sector->e_num);
		fputs(buffer, new_file);

		sprintf(buffer, "%f %f\n", current_sector->floor_height, current_sector->ceiling_height);
		fputs(buffer, new_file);

		EDGE * current_edge;

		for(int v = 0; v < current_sector->e_num; v++)
		{
			current_edge = current_sector->e + v;

			sprintf(buffer, "%u ", current_edge->v_start);
			fputs(buffer, new_file);
		}

		fputs("\n", new_file);

		sprintf(buffer, "%f %f %f\n", current_sector->tint.r, current_sector->tint.g, current_sector->tint.b);
		fputs(buffer, new_file);

		for(int v = 0; v < current_sector->e_num; v++)
		{
			current_edge = current_sector->e + v;

			sprintf(buffer, "%u %u %u %u %f %f\n", 	current_edge->text_param.id,
													current_edge->text_param.parallax,
													current_edge->text_param.u_offset,
													current_edge->text_param.v_offset,
													current_edge->text_param.u_scale,
													current_edge->text_param.v_scale);
			fputs(buffer, new_file);
		}

		sprintf(buffer, "%u %u %u %u %f %f\n", 	current_sector->text_param_ceil.id,
												current_sector->text_param_ceil.parallax,
												current_sector->text_param_ceil.u_offset,
												current_sector->text_param_ceil.v_offset,
												current_sector->text_param_ceil.u_scale,
												current_sector->text_param_ceil.v_scale);
		fputs(buffer, new_file);

		sprintf(buffer, "%u %u %u %u %f %f\n", 	current_sector->text_param_floor.id,
												current_sector->text_param_floor.parallax,
												current_sector->text_param_floor.u_offset,
												current_sector->text_param_floor.v_offset,
												current_sector->text_param_floor.u_scale,
												current_sector->text_param_floor.v_scale);
		fputs(buffer, new_file);
		fputs("\n", new_file);
	}

	fclose(new_file);

	CONSOLE_print("\nSaved to ");
	CONSOLE_print(filename);
}

void COMMAND_load_level(char * filename)
{
	level_load(filename);
}

