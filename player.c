#include "ff_vector2.h"
#include "world.h"
#include "engine.h"

#include "player.h"
/*
extern LEVEL loaded_level;

extern VECTOR2 editor_cursor;

extern int closest_vector_index;
extern int closest_edge_index;
extern int closest_sector_index;
extern VECTOR2 closest_edge_projection;

extern VECTOR2 closest_vector;
extern EDGE * closest_edge;
extern SECTOR * closest_sector;

extern float closest_vector_distance;
extern float closest_edge_distance;


*/

#include "gfx.h"
#include "camera.h"
#include "world.h"

void init_player()
{
	init_camera(&world.player.camera, 0.05, 10.0f, (float)gfx.screen_res_x/gfx.screen_res_y, M_PI/4.0f);

	world.player.position = vector2f(0.0f, 0.0f);
	world.player.current_height = 0.f;

	world.player.tallness = PLAYER_HEIGHT;
	world.player.h_velocity = 0.0f;
	
	world.player.is_grounded = true;

	world.player.facing = 0.0f;

	world.player.noclip = false;

	world.player.walk_speed = PLAYER_WALK_SPEED;
	world.player.run_speed = PLAYER_RUN_SPEED;

	world.player.walk_turn_speed = PLAYER_WALK_TURN_SPEED;
	world.player.run_turn_speed = PLAYER_RUN_TURN_SPEED;

	world.player.max_health = PLAYER_MAX_DEFAULT_HEALTH;
	world.player.health = PLAYER_MAX_DEFAULT_HEALTH;

	world.player.max_armor = PLAYER_MAX_DEFAULT_ARMOR;
	world.player.armor = PLAYER_MAX_DEFAULT_ARMOR;

	world.player.current_sector = NULL;
	world.player.closest_edge = NULL;
}

void update_player()
{

}

static inline float get_player_speed()
{
	if(world.player.is_running == true) return world.player.run_speed;
	else return world.player.walk_speed;
}

static inline float get_player_turn_speed()
{
	if(world.player.is_running == true) return world.player.run_turn_speed;
	else return world.player.walk_turn_speed;
}

void move_player(void* direction_)
{
	Vector2f amount = scale_v2(*(Vector2f*)direction_, get_player_speed() * engine_delta_time());

	world.player.position = sum_v2(world.player.position, amount);

	world.player.camera.position = world.player.position;
}

void turn_player(void* direction_)
{
	world.player.facing += *(float*)direction_ * get_player_turn_speed() * engine_delta_time();

	while(world.player.facing >= 2.0f * M_PI) world.player.facing -= 2.0f * M_PI;
	while(world.player.facing < 0.0f) world.player.facing += 2.0f * M_PI;

	world.player.camera.facing = world.player.facing;
}
/*
void PLAYER_Update()
{
	if(player->noclip == 0)
	{
		player->is_grounded = 0;

		if(player->h_velocity > 0.) 
			player->is_grounded = 0;
		else if(player->pos_height - player->height <= loaded_level.sectors[player->current_sector].floor_height)
		{
			player->is_grounded = 1;
			player->h_velocity = 0.;
			player->pos_height = loaded_level.sectors[player->current_sector].floor_height + player->height;	
		}

		if(player->is_grounded == 0)
		{
			player->h_velocity = player->h_velocity + (GRAVITY * engine_delta_time());
			player->pos_height = player->pos_height + (player->h_velocity * engine_delta_time());
		}
	}
}

void PLAYER_Move(PLAYER * player, VECTOR2 amount)
{
	SECTOR * current_sector;

	int allow_move = 1;

	VECTOR2 to_pos = sum_v2(player->pos, amount);

	current_sector = loaded_level.sectors + player->current_sector;

	VECTOR2 intersection;

	int dest_sector;
	int collision_state;

	if(!(player->movement_blocked))
	{

		collision_state = WORLD_Check_Collision(	player->current_sector, player->pos, amount, &intersection, &dest_sector,
													1, player->pos_height, player->height, PLAYER_KNEE);

		if(collision_state == COLLIDED)
		{
			allow_move = false;
		}
		else if(collision_state == NO_COLLISION_SECTOR_CHANGE)
		{
			player->current_sector = dest_sector;
			allow_move = true;
		}
		else
		{
			allow_move = true;
		}

		if(allow_move)
		{
			player->pos = to_pos;
			
			get_closest_vertex(player->pos, &player->closest_vector, &closest_vector_index, &player->closest_vector_distance);
			get_closest_edge(player->pos, &player->closest_edge, &closest_edge_projection, &closest_edge_index, &closest_sector_index, &player->closest_edge_distance);
			player->closest_sector = get_sector_at(closest_sector_index);
		}	
	}
}

void PLAYER_Turn(PLAYER * player, float amount)
{
	if(!player->movement_blocked)
	{
		player->facing = player->facing + amount;

		if(player->facing >= 2.*PI) player->facing -= 2*PI;
		if(player->facing < 0.) player->facing += 2*PI;
	}	
}

void PLAYER_Jump()
{
	if(player->is_grounded)
	{
		player->h_velocity = JUMP_VELOCITY;
	}
}
*/