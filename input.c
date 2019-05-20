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

extern bool game_mode;
extern bool edit_mode;

char* console_buffer;
int console_buffer_cursor_pos;

SDL_Event event;

char lower_case_symbols[20] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '[', ']', ';', '\'', ',', '.', '/'};
char upper_case_symbols[20] = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', ':', '\"', '<', '>', '?'};

extern clock_t current_frame_start;

extern PLAYER * player;

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
			player->speed = player->run_speed;
			player->turn_speed = player->run_turn_speed;
		}
		else
		{
			player->speed = player->walk_speed;
			player->turn_speed = player->walk_turn_speed;
		}

		if(keystate[SDLK_UP])
		{
			PLAYER_Move(player, scale_v2(rot_v2(vector2(0, 1), -(player->facing)), player->speed * ENGINE_delta_time()));
		}
		
		if(keystate[SDLK_DOWN])
		{
			PLAYER_Move(player, scale_v2(rot_v2(vector2(0, -1), -(player->facing)), player->speed * ENGINE_delta_time()));
		}
		
		if(keystate[SDLK_RIGHT])
		{
			if(keystate[SDLK_LALT])
			{
				PLAYER_Move(player, scale_v2(rot_v2(vector2(1, 0), -(player->facing)), player->speed * ENGINE_delta_time()));
			}
			else
			{
				player->facing = player->facing + player->turn_speed * ENGINE_delta_time();

				if(player->facing >= 2.*PI) player->facing -= 2*PI;
				if(player->facing < 0.) player->facing += 2*PI;
			}
		}
		
		if(keystate[SDLK_LEFT])
		{
			if(keystate[SDLK_LALT])
			{
				PLAYER_Move(player, scale_v2(rot_v2(vector2(-1, 0), -(player->facing)), player->speed * ENGINE_delta_time()));
			}
			else
			{
				player->facing = player->facing - player->turn_speed * ENGINE_delta_time();

				if(player->facing >= 2*PI) player->facing -= 2*PI;
				if(player->facing < 0.) player->facing += 2*PI;
			}
		}
		
		if(keystate[SDLK_PAGEUP])
		{
			player->pos_height += player->speed * ENGINE_delta_time();
		}
		
		if(keystate[SDLK_PAGEDOWN])
		{
			player->pos_height -= player->speed * ENGINE_delta_time();
		}
			
		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				e_running = false;
			}
			else if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{
					case '`':
						set_console_open(!get_console_open());
						break;

					case 'i':
						show_fps = !show_fps;
						break;

					case 'm':
						show_map = !show_map;
						break;

					case 'p':
						edit_mode = !edit_mode;
						game_mode = !game_mode;
						break;

					case '0':
						player->current_weapon = 0;
						break;
					case '1':
						player->current_weapon = 1;
						break;
					case '2':
						player->current_weapon = 2;
						break;
					case '3':
						player->current_weapon = 3;
						break;
					case '4':
						player->current_weapon = 4;
						break;
					case '5':
						player->current_weapon = 5;
						break;
					case '6':
						player->current_weapon = 6;
						break;
					case '7':
						player->current_weapon = 7;
						break;
					case '8':
						player->current_weapon = 8;
						break;
					case '9':
						player->current_weapon = 9;
						break;

					case SDLK_SPACE:
						PLAYER_Jump();
						break;
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