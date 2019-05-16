#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_HEIGHT 2.0f
#define PLAYER_CROUCH 1.0f

#define PLAYER_START_HEIGHT 0.5f
#define PLAYER_START_SPEED 1.0f

#define PLAYER_START_X 0.5f
#define PLAYER_START_Y 0.0f

#define PLAYER_MAX_DEFAULT_HEALTH 100
#define PLAYER_MAX_DEFAULT_ARMOR 100


typedef struct PLAYER_
{
	VECTOR2 pos;

	float height;

	float pos_height;

	float facing;

	int noclip;

	float walk_speed;
	float run_speed;

	float walk_turn_speed;
	float run_turn_speed;

	float speed;
	float turn_speed;

	int current_sector;

	int max_health;
	int health;

	int max_armor;
	int armor;
}
PLAYER;

void PLAYER_Init(PLAYER ** player);

void PLAYER_Move(PLAYER * player, VECTOR2 amount);

#endif