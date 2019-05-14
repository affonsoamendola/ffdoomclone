#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_HEIGHT 2.0f
#define PLAYER_CROUCH 1.0f

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
}
PLAYER;

void PLAYER_Init(PLAYER ** player);

void PLAYER_Move(PLAYER * player, VECTOR2 amount);

#endif