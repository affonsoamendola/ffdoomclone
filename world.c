
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

void delete_edge_at(int sector_index, int edge_index)
{
	EDGE * new_e;
	SECTOR * sector;

	sector = loaded_level.sectors + sector_index;

	new_e = malloc(sizeof(EDGE) * (sector->e_num-1));

	for(int i = 0; i < edge_index; i++)
	{
		new_e[i] = sector->e[i];
	}

	for(int i = edge_index + 1; i < sector->e_num; i++)
	{
		new_e[i-1] = sector->e[i];
	}

	sector->e_num -= 1;

	free(sector->e);

	if(sector->e_num == 0)
	{
		WORLD_delete_sector_at(sector_index);
	}
	else
	{
		sector->e = new_e;
	}
}

void WORLD_delete_sector_at(int index)
{
	SECTOR * new_sectors;

	new_sectors = malloc(sizeof(SECTOR) * (loaded_level.s_num - 1));

	for(int i = 0; i < index; i++)
	{
		new_sectors[i] = loaded_level.sectors[i];
	}

	for(int i = index + 1; i < loaded_level.s_num; i++)
	{
		loaded_level.sectors[i].sector_id--;
		new_sectors[i-1] = loaded_level.sectors[i];
	}

	free(loaded_level.sectors);

	loaded_level.sectors = new_sectors;
	loaded_level.s_num -= 1;
}

void WORLD_delete_vertex_at(int index)
{
	if(loaded_level.v_num > 0)
	{
		VECTOR2 * new_vertexes;

		new_vertexes = malloc(sizeof(VECTOR2) * (loaded_level.v_num - 1));

		for(int i = 0; i < index; i++)
		{
			new_vertexes[i] = loaded_level.vertexes[i];
		}

		for(int i = index + 1; i < loaded_level.v_num; i ++)
		{
			new_vertexes[i - 1] = loaded_level.vertexes[i];
		}

		loaded_level.v_num -= 1;

		free(loaded_level.vertexes);
		loaded_level.vertexes = new_vertexes;

		SECTOR * current_sector;
		EDGE * current_edge;
		EDGE * next_edge;
		int next_edge_index;

		int sector_amount;

		sector_amount = loaded_level.s_num;

		for(int s = 0; s < sector_amount; s++)
		{
			current_sector = loaded_level.sectors + s;

			for(int e = 0; e < current_sector->e_num; e++)
			{	
				current_edge = current_sector->e + e;

				if(e == current_sector->e_num - 1)
				{
					next_edge_index = 0;
				}
				else
				{
					next_edge_index = e + 1;
				}

				next_edge = current_sector->e + next_edge_index;

				if(current_edge->v_end == index)
				{
					current_edge->v_end = next_edge->v_end;

					delete_edge_at(s, next_edge_index);

					if(sector_amount > loaded_level.s_num)
					{
						sector_amount = loaded_level.s_num;
						s--;
						break;
					}
				}
			}
		}

		for(int s = 0; s < loaded_level.s_num; s++)
		{
			current_sector = loaded_level.sectors + s;

			for(int e = 0; e < current_sector->e_num; e++)
			{	
				current_edge = current_sector->e + e;

				if(current_edge->v_start > index)
					current_edge->v_start--;

				if(current_edge->v_end > index)
					current_edge->v_end--;
			}
		}
	}
}

EDGE * get_edge_from_level(int sector_index, int edge_index)
{
	SECTOR * sector;
	EDGE * edge;

	edge = NULL;

	if(sector_index < loaded_level.s_num)
	{
		sector = loaded_level.sectors + sector_index;
		
		if(edge_index < sector->e_num)
		{	
			edge = sector->e + edge_index;
		}
	}

	return edge;
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

			fscanf(level_file, "%u", &(new_edge.text_param.id));
			fscanf(level_file, "%u", &(new_edge.text_param.parallax));
			fscanf(level_file, "%u", &(new_edge.text_param.u_offset));
			fscanf(level_file, "%u", &(new_edge.text_param.v_offset));
			fscanf(level_file, "%f", &(new_edge.text_param.u_scale));
			fscanf(level_file, "%f", &(new_edge.text_param.v_scale));

			end_vertex = e + 1;

			if(end_vertex >= sector_size)
			{
				end_vertex = 0;
			}

			new_edge.v_end = *(sector_vertexes + end_vertex);

			new_edge.is_portal = false;
			new_edge.neighbor_sector_id = -1;

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

		fscanf(level_file, "%u", &(current_sector->text_param_ceil.id));
		fscanf(level_file, "%u", &(current_sector->text_param_ceil.parallax));
		fscanf(level_file, "%u", &(current_sector->text_param_ceil.u_offset));
		fscanf(level_file, "%u", &(current_sector->text_param_ceil.v_offset));
		fscanf(level_file, "%f", &(current_sector->text_param_ceil.u_scale));
		fscanf(level_file, "%f", &(current_sector->text_param_ceil.v_scale));

		fscanf(level_file, "%u", &(current_sector->text_param_floor.id));
		fscanf(level_file, "%u", &(current_sector->text_param_floor.parallax));
		fscanf(level_file, "%u", &(current_sector->text_param_floor.u_offset));
		fscanf(level_file, "%u", &(current_sector->text_param_floor.v_offset));
		fscanf(level_file, "%f", &(current_sector->text_param_floor.u_scale));
		fscanf(level_file, "%f", &(current_sector->text_param_floor.v_scale));

		current_sector->tint = GFX_Tint(1.f, 0.2f, 0.2f);
	}

	fclose(level_file);
}

int WORLD_add_vertex(VECTOR2 vertex)
{
	int new_index;

	new_index = loaded_level.v_num;

	VECTOR2 * new_vertexes;

	new_vertexes = malloc(sizeof(VECTOR2) * (new_index + 1));

	for(int i = 0; i < new_index; i++)
	{
		new_vertexes[i] = loaded_level.vertexes[i];
	}

	new_vertexes[new_index] = vertex;

	free(loaded_level.vertexes);
	loaded_level.vertexes = new_vertexes;

	loaded_level.v_num += 1;

	return new_index;
}

void WORLD_remove_n_vertexes(int n)
{
	VECTOR2 * new_vertexes;

	int new_vertexes_size;

	new_vertexes_size = loaded_level.v_num - n;

	new_vertexes = malloc(sizeof(VECTOR2) * (new_vertexes_size));

	for(int v = 0; v < new_vertexes_size; v ++)
	{
		new_vertexes[v] = loaded_level.vertexes[v];
	}

	loaded_level.v_num -= n;

	free(loaded_level.vertexes);

	loaded_level.vertexes = new_vertexes;
}

int WORLD_add_edge_to_sector(SECTOR * sector, int vertex_start_index, int vertex_end_index)
{
	int new_edge_index = sector->e_num;

	EDGE new_edge;

	EDGE * new_e;

	new_e = malloc(sizeof(EDGE) * (new_edge_index + 1));	

	for(int i = 0; i < new_edge_index; i++)
	{
		new_e[i] = sector->e[i];
	}

	new_edge.v_start = vertex_start_index;
	new_edge.v_end = vertex_end_index;

	new_edge.text_param = DEFAULT_TEXTURE_PARAM;

	new_edge.is_portal = 0;
	new_edge.neighbor_sector_id = -1;

	SECTOR * old_sector;
	EDGE * old_edge;

	for(int old_s = 0; old_s < loaded_level.s_num; old_s ++)
	{
		old_sector = loaded_level.sectors + old_s;

		for(int old_e = 0; old_e < old_sector->e_num; old_e ++)
		{
			old_edge = old_sector->e + old_e;

			if( (new_edge.v_start == old_edge->v_start && new_edge.v_end == old_edge->v_end) ||
				(new_edge.v_start == old_edge->v_end && new_edge.v_end == old_edge->v_start))
			{
				new_edge.is_portal = true;
				old_edge->is_portal = true;

				new_edge.neighbor_sector_id = old_s;
				old_edge->neighbor_sector_id = sector->sector_id;
			}
		}	
	}

	new_e[new_edge_index] = new_edge;

	free(sector->e);
	sector->e = new_e;

	sector->e_num = (sector->e_num) + 1;

	return new_edge_index;
}	

int WORLD_add_sector_to_level(SECTOR * sector)
{
	int new_sector_index = loaded_level.s_num;

	SECTOR * new_sectors;

	new_sectors = malloc(sizeof(SECTOR) * (new_sector_index + 1));

	for(int i = 0; i < new_sector_index; i++)
	{
		new_sectors[i] = loaded_level.sectors[i];
	}

	sector->sector_id = new_sector_index;
	sector->tint = GFX_Tint(1., 1., 1.);
	new_sectors[new_sector_index] = *sector;
	loaded_level.s_num += 1;

	free(loaded_level.sectors);
	loaded_level.sectors = new_sectors;

	return new_sector_index;
}

void get_closest_vertex(VECTOR2 pos, VECTOR2 * closest, int * vertex_index, float * distance)
{
	VECTOR2 current_vector;
	float current_distance;

	int closest_vector_i = 0;
	VECTOR2 closest_vector = loaded_level.vertexes[0];
	float closest_distance = norm_v2(sub_v2(pos, closest_vector));

	for(int v = 0; v < loaded_level.v_num; v++)
	{
		current_vector = loaded_level.vertexes[v];

		current_distance = norm_v2(sub_v2(pos, loaded_level.vertexes[v]));

		if(current_distance < closest_distance)
		{
			closest_vector_i = v;
			closest_vector = current_vector;
			closest_distance = current_distance;
		}
	}

	if(closest != NULL)
	{
		*closest = closest_vector;
	}

	if(vertex_index != NULL)
	{
		*vertex_index = closest_vector_i;
	}

	if(distance != NULL)
	{
		*distance = closest_distance;
	}
}

void get_closest_edge(VECTOR2 pos, EDGE ** edge, VECTOR2 * projection, int * edge_index, int * sector_index, float * distance)
{
	float closest_edge_index;
	float closest_sector_index;
	float closest_distance;
	VECTOR2 closest_projection;

	SECTOR * current_sector;
	EDGE * current_edge;
	VECTOR2 current_projection;

	float current_distance;

	current_distance = distance_v2_to_segment(pos, get_vertex_at(loaded_level.sectors->e->v_start), get_vertex_at(loaded_level.sectors->e->v_end), &current_projection);
	
	closest_edge_index = 0;
	closest_sector_index = 0;
	closest_distance = current_distance;
	closest_projection = current_projection;

	for(int s = 0; s < loaded_level.s_num; s++)
	{
		current_sector = loaded_level.sectors + s;

		for(int e = 0; e < current_sector->e_num; e++)
		{
			current_edge = current_sector->e + e;

			current_distance = distance_v2_to_segment(pos, get_vertex_at(current_edge->v_start), get_vertex_at(current_edge->v_end), &current_projection);
		
			if(	current_distance < closest_distance)
			{
				closest_edge_index = e;
				closest_sector_index = s;
				closest_distance = current_distance;
				closest_projection = current_projection;
			}
		}
	}

	if(edge != NULL)
		*edge = get_edge_from_level(closest_sector_index, closest_edge_index);

	if(projection != NULL)
		*projection = closest_projection;

	if(edge_index != NULL)
		*edge_index = closest_edge_index;

	if(sector_index != NULL)
		*sector_index = closest_sector_index;

	if(distance != NULL)
		*distance = closest_distance;
}