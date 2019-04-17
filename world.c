
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "console.h"
#include "vector2.h"

#include "world.h"

LEVEL loaded_level;

void WORLD_Init()
{
	CONSOLE_print("Initting World... Please hold...");
}

void level_load(const char * file_location)
{
	FILE * level_file;

	int vertex_num;
	int sector_num;

	int sector_size;

	float x;
	float y;

	level_file = fopen(file_location, "r");

	fscanf(level_file, "%u", &vertex_num);
	fscanf(level_file, "%u", &sector_num);

	loaded_level.v_num = vertex_num;
	loaded_level.vertexes = malloc(vertex_num * sizeof(VECTOR2));
	loaded_level.s_num = sector_num;
	loaded_level.sectors = malloc(sector_num * sizeof(SECTOR));

	for(int v = 0; v < vertex_num; v++)
	{
		fscanf(level_file, "%f %f", &x, &y);

		*(loaded_level.vertexes + v) = vector2(x, y);
	}

	for(int s = 0; s < sector_num; s++)
	{
		fscanf(level_file, "%u", &sector_size);
		(*(loaded_level.sectors + s)).v_num = sector_size;
		(*(loaded_level.sectors + s)).v = malloc(sector_size * sizeof(int));

		for(int sv = 0; sv < sector_size; sv++)
		{
			fscanf(level_file, "%u", (*(loaded_level.sectors + s)).v + sv);
		}
	}

	fclose(level_file);
}