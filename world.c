
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
	level_load("level");
}

VECTOR2 get_vertex_at(int index)
{
	if(index >= 0 && index < loaded_level.v_num)
	{
		return *(loaded_level.vertexes + index);
	}
	else
	{
		return vector2(0, 0);
	}
}

EDGE * get_edge_at(SECTOR * sector, int edge_index)
{
	if(edge_index >= 0 && edge_index < sector->e_num)
	{
		return sector->e + edge_index;
	}
	else
	{
		return sector->e;
	}
}

VECTOR2 get_vertex_from_sector(SECTOR * sector, int edge_index, int start_or_end)
{
	if(start_or_end == 0)
	{
		return get_vertex_at(get_edge_at(sector, edge_index)->v_start);
	}
	else
	{
		return get_vertex_at(get_edge_at(sector, edge_index)->v_end);
	}
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
		SECTOR * current_sector;

		current_sector = loaded_level.sectors + s;

		fscanf(level_file, "%u", &sector_size);

		current_sector->sector_id = s;
		current_sector->e_num = sector_size;
		current_sector->e = malloc(sector_size * sizeof(EDGE));

		fscanf(level_file, "%f", &(current_sector->floor_height));
		fscanf(level_file, "%f", &(current_sector->ceiling_height));
		
		int * sector_vertexes;

		sector_vertexes = malloc(sector_size * sizeof(int));

		for(int v = 0; v < sector_size; v++)
		{
			fscanf(level_file, "%u", sector_vertexes + v);
		}

		for(int e = 0; e < sector_size; e++)
		{
			EDGE new_edge;
			int end_vertex;

			new_edge.v_start = *(sector_vertexes + e);

			end_vertex = e + 1;

			if(end_vertex >= sector_size)
			{
				end_vertex = 0;
			}

			new_edge.v_end = *(sector_vertexes + end_vertex);

			new_edge.is_portal = false;

			for(int old_s = 0; old_s < s; old_s ++)
			{
				SECTOR * checking_sector;

				checking_sector = loaded_level.sectors + old_s;

				for(int old_e = 0; old_e < checking_sector->e_num; old_e++)
				{
					EDGE * checking_edge;

					checking_edge = checking_sector->e + old_e;

					if( (new_edge.v_start == checking_edge->v_start && new_edge.v_end == checking_edge->v_end) ||
						(new_edge.v_start == checking_edge->v_end && new_edge.v_end == checking_edge->v_start))
					{
						new_edge.is_portal = true;
						checking_edge->is_portal = true;

						new_edge.neighbor_sector_id = old_s;
						checking_edge->neighbor_sector_id = s;
					}
				}
			}

			*(current_sector->e + e) = new_edge;
		}
	}

	fclose(level_file);
}