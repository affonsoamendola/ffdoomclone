#include "vector2.h"
#include "world.h"

#include "player.h"

#define PLAYER_START_HEIGHT 2.0f
#define PLAYER_START_SPEED 1.0f

VECTOR2 player_pos = {0.5f, 0.0f};

float player_height = PLAYER_START_HEIGHT;

float player_pos_height = PLAYER_START_HEIGHT;

float player_facing = 0.0f;

float player_angle_cos = 1.0f;
float player_angle_sin = 0.0f;

float player_walk_speed = 2.0f;
float player_run_speed = 4.0f;

float player_walk_turn_speed = 1.0f;
float player_run_turn_speed = 3.0f;

float player_speed = 1.0f;
float player_turn_speed = 0.5f;

int current_player_sector = 0;

extern LEVEL loaded_level;

void player_move(VECTOR2 amount)
{
	SECTOR * current_sector;

	int allow_move = 1;

	VECTOR2 to_pos = sum_v2(player_pos, amount);

	current_sector = loaded_level.sectors + current_player_sector;

	for(int e = 0; e < current_sector->e_num; e ++)
	{
		EDGE * current_edge = get_edge_at(current_sector, e);

		VECTOR2 edge_vertex_0 = get_vertex_from_sector(current_sector, e, 0);
		VECTOR2 edge_vertex_1 = get_vertex_from_sector(current_sector, e, 1);

		if(intersect_box_v2(player_pos, to_pos, edge_vertex_0, edge_vertex_1))
		{
			if(point_side_v2(to_pos, edge_vertex_0, edge_vertex_1) < 0)
			{
				if(current_edge->is_portal)
				{
					allow_move = 1;

					current_player_sector = current_edge->neighbor_sector_id;
				}
				else
				{
					allow_move = 0;
				}
			}
		}
	}

	if(allow_move)
	{
		player_pos = to_pos;
	
		player_pos_height = current_sector->floor_height + player_height;
	}
}