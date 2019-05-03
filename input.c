#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "SDL.h"
#include "console.h"
#include "time.h"
#include "vector2.h"
#include "math.h"
#include "player.h"

#include "input.h"

extern bool e_running;

char* console_buffer;
int console_buffer_cursor_pos;

SDL_Event event;

char lower_case_symbols[20] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '[', ']', ';', '\'', ',', '.', '/'};
char upper_case_symbols[20] = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', ':', '\"', '<', '>', '?'};

extern clock_t current_frame_start;

extern VECTOR2 player_pos;

extern float player_walk_speed;
extern float player_walk_turn_speed;

extern float player_run_speed;
extern float player_run_turn_speed;

extern float player_pos_height;
extern float player_speed;
extern float player_turn_speed;
extern float player_facing;
extern float player_angle_cos;
extern float player_angle_sin;

bool show_fps = 0;
bool show_map = 0;

char get_upper_case_symbol(char lower_case)
{
	for(int i = 0; i < 20; i ++)
	{
		if(lower_case_symbols[i] == lower_case)
		{
			return upper_case_symbols[i];
		}
	}

	return lower_case;
}

void clear_console_buffer()
{
	for(int i = 0; i < CONSOLE_CHAR_LIMIT; i++)
	{
		*(console_buffer + i) = '\0';
	}
}

char* get_console_buffer()
{

	return console_buffer;
}

void INPUT_Handle_Console()
{
	while(SDL_PollEvent(&event) != 0)
	{
		if(event.type == SDL_QUIT)
		{
			e_running = false;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == '`')
			{
				set_console_open(!get_console_open());
			}
			else if(event.key.keysym.sym == SDLK_BACKSPACE && console_buffer_cursor_pos > 0)
			{
				*(console_buffer + console_buffer_cursor_pos - 1) = '\0';
				console_buffer_cursor_pos -= 1;
			}
			else if(event.key.keysym.sym == 32)
			{
				*(console_buffer + console_buffer_cursor_pos) = ' ';
				console_buffer_cursor_pos += 1;
			}
			else if(event.key.keysym.sym >= 97 && event.key.keysym.sym <= 122)
			{	
				if(SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT))
				{
					*(console_buffer + console_buffer_cursor_pos) = event.key.keysym.sym - 32;
				}
				else
				{
					*(console_buffer + console_buffer_cursor_pos) = event.key.keysym.sym;
				}
				
				console_buffer_cursor_pos += 1;
			}
			else if(event.key.keysym.sym >= 33 && event.key.keysym.sym <= 126)
			{
				if(SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT))
				{
					*(console_buffer + console_buffer_cursor_pos) = get_upper_case_symbol(event.key.keysym.sym);
				}
				else
				{
					*(console_buffer + console_buffer_cursor_pos) = event.key.keysym.sym;
				}

				console_buffer_cursor_pos += 1;
			}
			
			if(event.key.keysym.sym == SDLK_RETURN)
			{
				CONSOLE_scroll(1);
    			CONSOLE_print(console_buffer);
				parse_console(console_buffer);
				console_buffer_cursor_pos = 0;
				clear_console_buffer();
			}
		}
	}
}

void INPUT_Handle()
{
	float delta_time;

	delta_time = (float)(clock() - current_frame_start)/(float)CLOCKS_PER_SEC;

	if(get_console_open())
	{
		INPUT_Handle_Console();
	}
	else
	{
		unsigned char * keystate = SDL_GetKeyState(NULL); 

		if(keystate[SDLK_LSHIFT])
		{
			player_speed = player_run_speed;
			player_turn_speed = player_run_turn_speed;
		}
		else
		{
			player_speed = player_walk_speed;
			player_turn_speed = player_walk_turn_speed;
		}

		if(keystate[SDLK_UP])
		{
			player_move(scale_v2(rot_v2(vector2(0, 1), -player_facing), player_speed * ENGINE_delta_time()));
		}
		
		if(keystate[SDLK_DOWN])
		{

			player_move(scale_v2(rot_v2(vector2(0, -1), -player_facing), player_speed * ENGINE_delta_time()));

			
		}
		
		if(keystate[SDLK_RIGHT])
		{
			if(keystate[SDLK_LALT])
			{
				player_move(scale_v2(rot_v2(vector2(1, 0), -player_facing), player_speed * ENGINE_delta_time()));
			}
			else
			{
				player_facing = player_facing + player_turn_speed * ENGINE_delta_time();

				if(player_facing >= 2.*PI) player_facing -= 2*PI;
				if(player_facing < 0.) player_facing += 2*PI;

				//player_angle_cos = cos(player_facing);
				//player_angle_sin = sin(player_facing);
			}
		}
		
		if(keystate[SDLK_LEFT])
		{
			if(keystate[SDLK_LALT])
			{
				player_move(scale_v2(rot_v2(vector2(-1, 0), -player_facing), player_speed * ENGINE_delta_time()));
			}
			else
			{
				player_facing = player_facing - player_turn_speed * ENGINE_delta_time();

				if(player_facing >= 2*PI) player_facing -= 2*PI;
				if(player_facing < 0.) player_facing += 2*PI;

				//player_angle_cos = cos(player_facing);
				//player_angle_sin = sin(player_facing);
			}
		}
		
		if(keystate[SDLK_PAGEUP])
		{
			player_pos_height += player_speed * ENGINE_delta_time();
		}
		
		if(keystate[SDLK_PAGEDOWN])
		{
			player_pos_height -= player_speed * ENGINE_delta_time();
		}
			
		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				e_running = false;
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == '`')
				{
					set_console_open(!get_console_open());
				}
				else if(event.key.keysym.sym == 'p')
				{
					show_fps = !show_fps;
				}
				else if(event.key.keysym.sym == 'm')
				{
					show_map = !show_map;
				}
			}
		}
	}	
}

void INPUT_Init()
{
	console_buffer = malloc(CONSOLE_CHAR_LIMIT*sizeof(char));

	clear_console_buffer();

	console_buffer_cursor_pos = 0;
}

void INPUT_Quit()
{
	free(console_buffer);
}