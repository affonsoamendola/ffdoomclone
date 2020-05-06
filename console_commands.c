#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "world.h"
#include "console.h"
#include "vector3.h"
#include "player.h"

#include "ff_math.h"

void command_ver(char** tokens)
{

	printf_console("RT76800\nVersion %s\n", ENGINE_VERSION);
}

void command_intro(char** tokens)
{
	printf_console("----------------------------------------\n");
	printf_console("Foffonso's DoomClone Engine\n");
	printf_console("Codename DN F3D\n");
	printf_console("Copyright Affonso Amendola, 2019-2020\n");	
	printf_console("Version %s\n\n", ENGINE_VERSION);
	printf_console("Be Excellent to Each Other\n");
	printf_console("----------------------------------------\n");
}

void command_help(char** tokens)
{
	int page;

	sscanf(*tokens, "%d", &page);

	if(page <= 1)
	{
		printf_console("----------------------------------------\n");
		printf_console("Command List Page 1/3\n");
		printf_console("help [page]:\n");
		printf_console("  shows command list's specified page\n");
		printf_console("ver: shows current game version\n");
		printf_console("intro: shows console intro again\n");
		printf_console("sector show [sector value]:\n");
		printf_console("  shows the sector's vertexes info\n");
		printf_console("noclip: enables no-clip mode\n");
		printf_console("----------------------------------------\n");
	}
	else
	if(page == 2)
	{
		printf_console("----------------------------------------\n");
		printf_console("Command List Page 2/3\n");
		printf_console("vertex list: \n");
		printf_console("  list all vertexes in the current level\n");
		printf_console("set tint [r] [g] [b]:\n");
		printf_console("  sets the tint value of the player's   \n");
		printf_console("  sector to the [r] [g] [b] values      \n");
		printf_console("  specified, from 0 to 1\n");
		printf_console("set health [value]: sets health to value\n");
		printf_console("----------------------------------------\n");
	}
	else
	if(page >= 3)
	{
		printf_console("----------------------------------------\n");
		printf_console("Command List Page 3/3\n");
		printf_console("set armor [value]: sets armor to value\n");
		printf_console("edit: enables edit mode\n");
		printf_console("save [filename]: \n");
		printf_console("  saves current edited level as filename\n");
		printf_console("load [filename]: \n");
		printf_console("  loads level from filename\n");
		printf_console("\n");
		printf_console("----------------------------------------\n");
	}
}

void console_print_vertex_at(int index)
{
	Vertex vert = *(Vertex*)ff_get_at_list(&world.vertexes, index);
	index %= world.vertexes.size;
	printf_console("%d x = %.2f, y = %.2f\n", index, vert.value.x, vert.value.y);
}

void command_vertex(char** tokens)
{
	int index;

	sscanf(*tokens, "%d", &index);

	console_print_vertex_at(index);
}

void command_remove_vertex(char** tokens)
{
	int index;

	sscanf(*tokens, "%d", &index);

	level_destroy_vertex(ff_get_at_list(&world.vertexes, index));
	scroll_console(1);
}


void command_vertex_list(char** tokens)
{
	int index;

	sscanf(*tokens, "%d", &index);

	for(uint32_t i = 0; i < ff_min(world.vertexes.size, 12); i++)
	{
		console_print_vertex_at(index);

		index++;
		index %= world.vertexes.size;
	}

	if(world.vertexes.size == 0) printf_console("No vertexes, dude.\n");
}
/*
void command_sector_show(char** tokens)
{
	int sector_index;

	sscanf(*tokens, "%d", &sector_index);

	if(sector_index >= 0 && sector_index < loaded_level.s_num)
	{
		printf_console("\nx  y  is_portal neighbor_sector_id");

		for(int e = 0; e < (loaded_level.sectors+sector_index)->e_num; e++)
		{
			EDGE current_edge;

			current_edge = *(((loaded_level.sectors+sector_index)->e+e));

			sprintf(buffer, "\n%u  %u  %u         %u", current_edge.v_start, current_edge.v_end, current_edge.is_portal, current_edge.neighbor_sector_id);
			printf_console(buffer);
		}
	}
	printf_console("\n");
}

void COMMAND_vertex_list(char** tokens)
{
	for(int v = 0; v < loaded_level.v_num; v++)
	{
		sprintf(buffer, "\n%f %f", (loaded_level.vertexes + v)->x, (loaded_level.vertexes + v)->y);
		printf_console(buffer);
	}
}

void COMMAND_noclip(char** tokens)
{
	player->noclip = !(player->noclip);

	if(player->noclip == 1)
		printf_console("\nFLY YOU FOOL");

	if(player->noclip == 0)
		printf_console("\nYOU ARE GROUNDED");
}

void COMMAND_set(char** tokens)
{
	char* variable = tokens[0];
	int value;

	sscanf(tokens[1], "%d", &value);

	if(strcmp(variable, "health") == 0)
	{
		printf_console("\nSetting health to %i", value);

		player->health = value;
	}

	if(strcmp(variable, "armor") == 0)
	{
		printf_console("\nSetting armor to %i");
		player->armor = value;
	}
}

void COMMAND_set_tint(char** tokens)
{
	float r;
	float g;
	float b;

	sscanf(tokens[0], "%f", &r);
	sscanf(tokens[1], "%f", &g);
	sscanf(tokens[2], "%f", &b);

	player->closest_sector->tint = GFX_Tint(r, g, b);
}

void COMMAND_save_level(char** tokens)
{
	char* filename = *tokens;

	printf_console("\nSaving...");
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

	printf_console("\nSaved to %s", filename);
}

void COMMAND_load_level(char** tokens)
{
	level_load(*tokens);
}

void COMMAND_edit()
{
	engine.edit_mode = !engine.edit_mode;
	player->noclip = !player->noclip;
}*/