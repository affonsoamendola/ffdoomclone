
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
	CONSOLE_print("\nFofonso's Raytracing Engine\n");
	CONSOLE_print("\nCodename RT76800");
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
		CONSOLE_print("\n");

		for(int v = 0; v < (loaded_level.sectors+sector_index)->v_num; v++)
		{
			sprintf(buffer, "%u ", *(((loaded_level.sectors+sector_index)->v+v)));
			CONSOLE_print(buffer);
		}
	}
}

void COMMAND_vertex_list()
{
	for(int v = 0; v < loaded_level.v_num; v++)
	{
		sprintf(buffer, "\n%f %f", (loaded_level.vertexes + v)->x, (loaded_level.vertexes + v)->y);
		CONSOLE_print(buffer);
	}
}