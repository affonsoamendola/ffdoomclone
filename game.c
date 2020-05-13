#include "engine.h"
#include "game.h"
#include "gfx.h"
#include "3d.h"
#include "world.h"
#include "player.h"
#include "input.h"
#include "console.h"

#include "ff_vector2.h"
#include "ff_color.h"
#include "ff_point2.h"

Game game;

Vector2f 	FORWARD_DIRECTION 		= vector2f( 0.0f,  1.0f);
Vector2f	BACKWARD_DIRECTION 		= vector2f( 0.0f, -1.0f);
Vector2f 	RIGHT_DIRECTION 		= vector2f( 1.0f,  0.0f);
Vector2f 	LEFT_DIRECTION 			= vector2f(-1.0f,  0.0f);
float    	TURN_DIRECTION_RIGHT	=  1.0f;
float	   	TURN_DIRECTION_LEFT  	= -1.0f;

//Relates an Action to a function (And its data)
Action game_actions[] =
{
	{ACTION_QUIT, 				signal_quit, 	&engine,  0},
	{ACTION_TOGGLE_CONSOLE, 	toggle_console, &console, 0},
	{ACTION_CONFIRM_CONSOLE,	enter_console, 	&input.text_input_character_loc, 0},
	{ACTION_FORWARD, 			move_player, 	&FORWARD_DIRECTION, ACT_FLAG_CONTINUOUS},
	{ACTION_BACKWARD, 			move_player, 	&BACKWARD_DIRECTION, ACT_FLAG_CONTINUOUS},
	{ACTION_STRAFE_RIGHT, 		move_player, 	&RIGHT_DIRECTION, ACT_FLAG_CONTINUOUS},
	{ACTION_STRAFE_LEFT, 		move_player, 	&LEFT_DIRECTION, ACT_FLAG_CONTINUOUS},
	{ACTION_TURN_RIGHT, 		turn_player, 	&TURN_DIRECTION_RIGHT, ACT_FLAG_CONTINUOUS},
	{ACTION_TURN_LEFT, 			turn_player, 	&TURN_DIRECTION_LEFT, ACT_FLAG_CONTINUOUS}
};
const static uint32_t game_actions_size = sizeof(game_actions)/sizeof(Action);

void init_game()
{
	set_input_actions(game_actions, game_actions_size);
}

void quit_game()
{	
}

void update_game()
{
}

void draw_game()
{
	render_level(&world.player.camera);

	if(engine.debug_flag)
	{
		GFX_draw_string_color_f(point2(4, 220), 3, DEBUG_TEXT_COLOR, "px= %+-4.2f py= %+-4.2f", world.player.position.x, world.player.position.y);
	}
}
