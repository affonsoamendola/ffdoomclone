#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_HEIGHT 2.0f
#define PLAYER_CROUCH 1.0f

#define PLAYER_KNEE 0.3f //Value suggested by Bruno Tonsa, any complaints must be directed to
						 //bruno@tonsa.org

#define JUMP_VELOCITY 3.0f

#define GRAVITY -10.f

#define PLAYER_START_HEIGHT 0.5f
#define PLAYER_START_SPEED 1.0f

#define PLAYER_START_X 0.5f
#define PLAYER_START_Y 0.0f

#define PLAYER_MAX_DEFAULT_HEALTH 100
#define PLAYER_MAX_DEFAULT_ARMOR 100

#define WEAPON_0_MAX_AMMO 10
#define WEAPON_1_MAX_AMMO 0
#define WEAPON_2_MAX_AMMO 150
#define WEAPON_3_MAX_AMMO 100
#define WEAPON_4_MAX_AMMO 400
#define WEAPON_5_MAX_AMMO 120
#define WEAPON_6_MAX_AMMO 150
#define WEAPON_7_MAX_AMMO 110
#define WEAPON_8_MAX_AMMO 50
#define WEAPON_9_MAX_AMMO 25

typedef struct PLAYER_
{
	VECTOR2 pos;

	float height;

	float pos_height;
	float h_velocity;

	int is_grounded;

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

	int current_weapon;

	int ammo[10];
	int max_ammo[10];
}
PLAYER;

void PLAYER_Init(PLAYER ** player);

void PLAYER_Update();

void PLAYER_Jump();
void PLAYER_Move(PLAYER * player, VECTOR2 amount);

#endif