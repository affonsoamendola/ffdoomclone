
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "world.h"
#include "console.h"
#include "vector3.h"

char buffer[128];

extern LEVEL loaded_level;

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