#include <stdio.h>
#include <stdlib.h>

//#include "list.h"
//#include "console.h"
#include "ff_vector2.h"
//#include "player.h"
#include "ff_linked_list.h"

#include "world.h"
#include "player.h"

World world;

void init_world()
{
	ff_initialize_list(&world.vertexes);
	ff_initialize_list(&world.edges);
	ff_initialize_list(&world.sectors);
	ff_initialize_list(&world.entities);

	init_player();
}

void quit_world()
{
	ff_destroy_list(&world.vertexes);
	ff_destroy_list(&world.edges);
	ff_destroy_list(&world.sectors);
	ff_destroy_list(&world.entities);
}

void level_add_edge(Vertex* start, Vertex* end)
{
	Edge new_edge = {start, end, {0}, 0, {NULL,NULL}};
	ff_pushback_list(&world.edges, &new_edge);
}

void level_add_vertex(Vector2f new_vertex_)
{
 	Vertex new_vertex = {new_vertex_};
	ff_pushback_list(&world.vertexes, &new_vertex);
}

//Destroyers removers of lists
void level_destroy_sector(Sector* sector)
{
	uint32_t found_index;
	if(ff_find_list(&world.sectors, &found_index, sector))
	{
		ff_remove_at_list(&world.sectors, found_index);
	}
}

void level_destroy_edge(Edge* edge)
{
	uint32_t found_index;
	if(ff_find_list(&world.edges, &found_index, edge))
	{
		ff_remove_at_list(&world.edges, found_index);
	}
}

void level_destroy_vertex(Vertex* vertex)
{
	uint32_t found_index;
	if(ff_find_list(&world.vertexes, &found_index, vertex))
	{
		ff_remove_at_list(&world.vertexes, found_index);
	}
}
//End destroyers

//Removes a sector from the level, does all the hierarquical solution for the lower
//levels, goes trough vertexes and edges and removes those who dont need to exist anymore.
//Also removes the neighbor information from edges that dont need it anymore.
void level_remove_sector(Sector* sector)
{
	bool is_last_edge_portal = sector->edges[sector->edge_size-1]->is_portal;

	//Goes through every edge in the sector
	for(int e = 0; e < sector->edge_size; e++)
	{
		Edge* current_edge = sector->edges[e];

		//If last and current edge arent portals, it means the vertex between them
		//is only used by this sector, and can be destroyed.
		if(!is_last_edge_portal && !current_edge->is_portal)
		{
			level_destroy_vertex(current_edge->vertex_start);
		}

		//If edge is not a portal, it means that it is only used by this sector and can be destroyed.
		if(!current_edge->is_portal)
		{
			level_destroy_edge(current_edge);
		}
		else //If IT IS a portal then it wont be after this sector is removed, and that edge wont neighbor this sector anymore.
		{
			current_edge->is_portal = false;

			if(current_edge->neighboring_sectors[0] == sector) current_edge->neighboring_sectors[0] = NULL;
			if(current_edge->neighboring_sectors[1] == sector) current_edge->neighboring_sectors[1] = NULL;
		}
	}

	//Frees the edges array, since it was malloced on creation.
	free(sector->edges);

	level_destroy_sector(sector);
}
/*
void level_load(const char * file_location)
{
	FILE * level_file;

	LEVEL new_level;

	int success = 1;

	int vertex_num;
	int sector_num;

	int sector_size;

	float x;
	float y;

	level_file = fopen(file_location, "r");

	if(level_file == NULL)
	{
		success = 0;
	}
	else
	{
		fscanf(level_file, "%u", &vertex_num);
		fscanf(level_file, "%u", &sector_num);

		new_level.v_num = vertex_num;
		new_level.vertexes = malloc(vertex_num * sizeof(VECTOR2));
		new_level.s_num = sector_num;
		new_level.sectors = malloc(sector_num * sizeof(SECTOR));

		for(int v = 0; v < vertex_num; v++)
		{
			fscanf(level_file, "%f %f", &x, &y);

			*(new_level.vertexes + v) = vector2(x, y);
		}

		for(int s = 0; s < sector_num; s++)
		{
			SECTOR * current_sector;

			current_sector = new_level.sectors + s;

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

			float r;
			float g;
			float b;

			fscanf(level_file, "%f %f %f", &r, &g, &b);

			current_sector->tint = GFX_Tint(r,g,b);

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

					checking_sector = new_level.sectors + old_s;

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
		}

		fclose(level_file);
	}

	if(success == 1)
	{
		loaded_level = new_level;
		printf_console("\nLoaded ");
		printf_console((char *)file_location);
	}
	else
	{
		printf_console("\nError loading level at ");
		printf_console((char *)file_location);
	}
}
*/

//Gets the closest vertex in the world for that position
//Puts NULL in closest and 0 in distance if cant find one (Basically means the closest is itself.)
void level_closest_vertex(const Vector2f pos, Vector2f** closest_out, float * distance_out)
{
	float closest_distance = FLT_MAX;
	Vector2f* closest_vector = NULL;

	for(int i=0; i < world.vertexes.size; i++) //world.vertexes.size guarantees(hopefully) that the carouselling will go back to the first element being first.
	{
		//Standard closest checking.
		Vector2f* current_vector = (Vector2f*)ff_get_at_list(&world.vertexes, 0);
		float current_distance = distance_v2(pos, *current_vector);

		if(current_distance < closest_distance)
		{
			closest_distance = current_distance;
			closest_vector = current_vector;
		}
		
		//Uses vertex as a carrousel, moving the first element pointer forward.
		ff_move_carrousel(&world.vertexes, 1);
	}

	if(closest_vector == NULL)
	{
		*distance_out = 0.0f;
		*closest_out = NULL;
	}
	else
	{
		*distance_out = closest_distance;
		*closest_out = closest_vector;
	}
}

void level_closest_edge(	const Vector2f pos, 
							Edge** closest_out, 
							Vector2f* projection_out,  
							float* distance_out)
{
	float closest_distance = FLT_MAX;
	Vector2f closest_projection = {0,0};
	Edge* closest_edge = NULL;

	//Standard closest checking.
	for(int i=0; i < world.edges.size; i++)
	{
		Edge* current_edge = (Edge*)ff_get_at_list(&world.edges, 0);
		Vector2f current_projection;
		float current_distance = distance_to_segment_v2(pos, 
														current_edge->vertex_start->value,
														current_edge->vertex_end->value,
														&current_projection);

		if(current_distance < closest_distance)
		{
			closest_distance = current_distance;
			closest_projection = current_projection;
			closest_edge = current_edge;
		}
		
		//Same thing as in closest vertex.
		ff_move_carrousel(&world.edges, 1);
	}

	if(closest_edge == NULL)
	{
		*distance_out = 0.0f;
		*projection_out = vector2f(0,0);
		*closest_out = NULL;
	}
	else
	{
		*distance_out = closest_distance;
		*projection_out = closest_projection;
		*closest_out = closest_edge;
	}
}
/*
//Checks collision in the world, takes a lot of parameters,
//start_sector, is the sector which the start_pos is located, move amount is a vector which shows how much to move from the start pos
//intersected pos is a pointer to a vector 2, will be filled with a position which is an intersection of start_pos+move_amounte and a wall
//check_knees will check if a ledge is above a certain knee_height value, if it is, it wont allow movement and act like a wall
//pos height is the current height of start_pos, and height is how tall its hitbox is, to check for head collisions with inverted ledges.

//Returns NO_COLLISION if no collision, COLLIDED if collision, and NO_COLLISION_SECTOR_CHANGE if 
int WORLD_Check_Collision(	int start_sector, VECTOR2 start_pos, VECTOR2 move_amount, VECTOR2 * intersected_position, int * end_sector, 
							int check_knees, float pos_height, float height, float knee_height)
{
	SECTOR * current_sector; 
	EDGE * current_edge;
	EDGE * neighbor_edge;

	VECTOR2 intersection_location;

	VECTOR2 to_location;

	to_location = sum_v2(start_pos, move_amount);

	current_sector = get_sector_at(start_sector);
	int changed_sector = 0;

	for(int e = 0; e < current_sector->e_num; e++)
	{
		current_edge = get_edge_at(current_sector, e);

		if(intersect_check_v2(start_pos, to_location, get_vertex_from_sector(current_sector, e, 0),  get_vertex_from_sector(current_sector, e, 1), &intersection_location) 
			== 1)
		{
			if(current_edge->is_portal)
			{
				for(int i  = 0; i < 3; i++)
				{
					SECTOR * neighbor_sector = get_sector_at(current_edge->neighbor_sector_id);

					if(check_knees)
					{
						if(pos_height + knee_height > neighbor_sector->floor_height)
						{
							changed_sector = 1;
							*end_sector = current_edge->neighbor_sector_id;
						}
						else return COLLIDED;
					}
					else
					{
						if(pos_height > neighbor_sector->floor_height)
						{
							changed_sector = 1;
							*end_sector = current_edge->neighbor_sector_id;
						}
						else return COLLIDED;
					}

					for(int e_n = 0; e_n < neighbor_sector->e_num; e_n++)
					{
						neighbor_edge = get_edge_at(neighbor_sector, e_n);

						if(neighbor_edge->neighbor_sector_id != current_sector->sector_id)
						{
							if(intersect_check_v2(start_pos, to_location, get_vertex_from_sector(neighbor_sector, e_n, 0),  get_vertex_from_sector(neighbor_sector, e_n, 1), &intersection_location) 
								== 1)
							{
								if(neighbor_edge->is_portal) 
								{
									current_sector = neighbor_sector;
									current_edge = neighbor_edge;
									break;
								}
								else
								{
									return COLLIDED;
								}
							}
						}
					}
				}
			}
		 	else
			{
				return COLLIDED;
			}
			break;	
		}
	}

	if(changed_sector) return NO_COLLISION_SECTOR_CHANGE;
	else return NO_COLLISION;
}
*/