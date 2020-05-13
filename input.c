#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "input.h"
#include "console.h"

#include "ff_vector2.h"

//Relates a Key to an Action
ActionKeyPair action_dictionary[] = 
{
	{SDLK_ESCAPE, ACTION_QUIT},
	{SDLK_BACKQUOTE, ACTION_TOGGLE_CONSOLE},
	{SDLK_RETURN, ACTION_CONFIRM_CONSOLE},
	{SDLK_KP_ENTER, ACTION_CONFIRM_CONSOLE},
	{SDLK_w, ACTION_FORWARD}, 	
	{SDLK_s, ACTION_BACKWARD}, 		
	{SDLK_d, ACTION_STRAFE_RIGHT}, 		
	{SDLK_a, ACTION_STRAFE_LEFT}, 		
	{SDLK_RIGHT, ACTION_TURN_RIGHT}, 	
	{SDLK_LEFT, ACTION_TURN_LEFT}
};
const static uint32_t action_dictionary_size = sizeof(action_dictionary)/sizeof(action_dictionary[0]);

//Relates an Action to a function (And its data)
Action default_registered_actions[] =
{
	{ACTION_QUIT, signal_quit, &engine},
	{ACTION_TOGGLE_CONSOLE, toggle_console, &console},
	{ACTION_CONFIRM_CONSOLE, enter_console, &input.text_input_character_loc}
};
const static uint32_t default_registered_actions_size = sizeof(default_registered_actions)/sizeof(Action);

Input input;

//Inits state.
void init_input()
{
	//Gets a snapshot of the keyboardstate
	input.is_scancode_down = SDL_GetKeyboardState(NULL);
	input.mouse_sensitivity = 0.4f;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	set_input_actions(default_registered_actions, default_registered_actions_size);

	input.text_input_enabled = false;
	input.text_input_destination = NULL;
	input.text_input_buffer_size = 0;
	input.text_input_character_loc = 0;

	input.disabled_actions = NULL;
	input.disabled_actions_size = 0;

}

//Called once per frame
void update_input()
{
	SDL_Event event;

	input.mouse_buttons = SDL_GetMouseState(&input.mouse_x, &input.mouse_y);
	

	//Polls all events (also updates input.is_scancode_down by calling SDL_PumpEvents internally)
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				signal_quit(&engine);
				break;
			case SDL_KEYDOWN:
				on_keydown(event.key.keysym.sym, false);
				break;
			case SDL_MOUSEMOTION:
				on_mouse_movement(scale_v2(vector2f(event.motion.xrel, -event.motion.yrel), input.mouse_sensitivity));
				break;
			case SDL_MOUSEWHEEL:
				on_mouse_wheel(event.wheel.y);
				break;
		}		
	}

	if(!input.text_input_enabled)
	{
		for(int i = 0; i < 284; i++)
		{
			if(input.is_scancode_down[i])
			{
				on_keydown(SDL_GetKeyFromScancode(i), true);
			}
		}
	}
}

//Returns true if keycode is in the dictionary and puts the corresponding ActionCode in out_code
bool get_actioncode_from_keycode(SDL_Keycode keycode, ActionCode* outcode)
{
	for(int i = 0; i < action_dictionary_size; i++)
	{
		if(action_dictionary[i].keycode == keycode)
		{
			*outcode = action_dictionary[i].actioncode;
			
			return true;
		}
	}
	return false;
}

//Returns true if actioncode is in the dictionary and puts the corresponding Keycode in out_code
bool get_keycode_from_actioncode(ActionCode actioncode, SDL_Keycode* outcode)
{
	for(int i = 0; i < action_dictionary_size; i++)
	{
		if(action_dictionary[i].actioncode == actioncode)
		{
			*outcode = action_dictionary[i].keycode;
			return true;
		}
	}
	return false;
}
//Sets current input action context.
void set_input_actions(Action* actions, const uint32_t number_of_actions)
{
	if(actions == NULL)
	{
		input.registered_actions = default_registered_actions;
		input.registered_actions_size = default_registered_actions_size;
	}
	else
	{
		input.registered_actions = actions;
		input.registered_actions_size = number_of_actions;
	}
}

//Disables some actions from being triggered.
void set_disabled_actions(ActionCode* actions, const uint32_t number_of_actions)
{
	if(actions == NULL)
	{
		input.disabled_actions = NULL;
		input.disabled_actions_size = 0;
	}
	else
	{
		input.disabled_actions = actions;
		input.disabled_actions_size = number_of_actions;
	}	
}

//Disables some actions from being triggered.
void set_enabled_actions(ActionCode* actions, const uint32_t number_of_actions)
{
	if(actions == NULL)
	{
		input.enabled_actions = NULL;
		input.enabled_actions_size = 0;
	}
	else
	{
		input.enabled_actions = actions;
		input.enabled_actions_size = number_of_actions;
	}	
}

//Returns true if ActionCode is in the disabled actions list.
bool check_disabled_actions(ActionCode action)
{
	for(int i = 0; i < input.disabled_actions_size; i++)
	{
		if(action == input.disabled_actions[i])
		{
			return true;
		}
	}

	return false;
}

//Returns true if ActionCode is in the enabled actions list.
bool check_enabled_actions(ActionCode action)
{
	if(input.enabled_actions == NULL) return true;

	for(int i = 0; i < input.enabled_actions_size; i++)
	{
		if(action == input.enabled_actions[i])
		{
			return true;
		}
	}

	return false;
}

//Performs action of action_code specified (Only keydowns)
void on_action(Action action)
{
	if(!check_disabled_actions(action.action) &&
		check_enabled_actions(action.action))
	{
		action.function.default_func(action.user_data);
	}
}

bool check_registered_actions(ActionCode code, Action* out)
{
	for(int i = 0; i < input.registered_actions_size; i++)
	{
		if(	code == input.registered_actions[i].action )
		{
			*out = input.registered_actions[i];
			return true;
		}
	}

	return false;
}

//Searches the Action Dictionary and Registered actions for the key that was pressed
//And executes it.
void on_keydown(SDL_Keycode keycode, bool continuous)
{
	//If text input is enabled, write to text out.
	if(input.text_input_enabled) on_text_input(keycode);

	bool has_action = false;

	ActionCode found_action_code;
	Action found_action;

	if(!get_actioncode_from_keycode(keycode, &found_action_code)) return;

	has_action = check_registered_actions(found_action_code, &found_action);
	
	//Do nothing if on continuous mode and action is not continuous
	if(continuous && !(found_action.flags & ACT_FLAG_CONTINUOUS)) return;

	//This is the normal on_action
	if(has_action) on_action(found_action);
}

//Performs a Mouse_movement action.
void on_mouse_movement(const Vector2f amount)
{
	if(check_disabled_actions(ACTION_MOUSE_MOVE) || !check_enabled_actions(ACTION_MOUSE_MOVE)) return;

	for(int i = 0; i < input.registered_actions_size; i++)
	{
		if(input.registered_actions[i].action == ACTION_MOUSE_MOVE)
		{
			input.registered_actions[i].function.mouse_move_func(input.registered_actions[i].user_data, amount);
		}

		if(	input.registered_actions[i].action == ACTION_MOUSE_DRAG_RIGHT && 
			is_mouse_held(INPUT_MOUSE_RIGHT))
		{
			input.registered_actions[i].function.mouse_move_func(input.registered_actions[i].user_data, neg_v2(amount));
		}
	}
}

void on_mouse_wheel(const int direction)
{
	if(check_disabled_actions(ACTION_SCROLL_WHEEL) || !check_enabled_actions(ACTION_SCROLL_WHEEL)) return;

	for(int i = 0; i < input.registered_actions_size; i++)
	{
		if(input.registered_actions[i].action == ACTION_SCROLL_WHEEL)
		{
			input.registered_actions[i].function.mouse_wheel_func(input.registered_actions[i].user_data, direction);
		}
	}
}

void on_text_input(SDL_Keycode keycode)
{
	char current_char;

	SDL_Keycode console_open_keycode;

	get_keycode_from_actioncode(ACTION_TOGGLE_CONSOLE, &console_open_keycode);

	current_char = keycode;

	if(keycode >= 0x20 && keycode <= 0x7E && keycode != console_open_keycode)
	{
		if(keycode >= 0x61 && keycode <= 0x7A && (SDL_GetModState() & KMOD_SHIFT))
		{
			current_char = keycode - 0x20;
		}

		if(input.text_input_character_loc < input.text_input_buffer_size)
		{
			*(input.text_input_destination + input.text_input_character_loc) = current_char; 
			input.text_input_character_loc++;
		}
	}

	if(current_char == '\b' && input.text_input_character_loc > 0)
	{
		input.text_input_character_loc -= 1;
		*(input.text_input_destination + input.text_input_character_loc) = ' ';
	}
}

void start_text_input(char* text_destination, uint32_t buffer_size)
{
	input.text_input_enabled = true;
	input.text_input_destination = text_destination;
	input.text_input_buffer_size = buffer_size;
	input.text_input_character_loc = 0;
}

void end_text_input()
{
	input.text_input_enabled = false;
	input.text_input_destination = NULL;
	input.text_input_buffer_size = 0;
	input.text_input_character_loc = 0;
}

/*
#include "SDL2/SDL.h"
#include "console.h"
#include "time.h"
#include "vector2.h"
#include "math.h"
#include "editor.h"
#include "player.h"
#include "world.h"

#include "input.h"
/*
extern bool e_running;

extern bool game_mode;
extern bool edit_mode;

extern int show_texture_select;
extern int selecting_texture_for;

char* console_buffer;
int console_buffer_cursor_pos;

SDL_Event event;

char lower_case_symbols[20] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '[', ']', ';', '\'', ',', '.', '/'};
char upper_case_symbols[20] = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', ':', '\"', '<', '>', '?'};

extern clock_t current_frame_start;

extern PLAYER * player;

bool show_fps = 0;
bool show_map = 0;

extern VECTOR2 closest_vector;
extern EDGE * closest_edge;
extern SECTOR * closest_sector;

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
			engine.running = false;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == '`' || event.key.keysym.sym == '\'' || event.key.keysym.sym == 'q')
			{
				set_console_open(!is_console_open());
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
				printf_console("\n");
    			printf_console(console_buffer);
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

	delta_time = (float)(clock() - engine.current_frame_start)/(float)CLOCKS_PER_SEC;

	if(is_console_open())
	{
		INPUT_Handle_Console();
	}
	else if(engine.edit_mode == 1)
	{
		EDIT_MODE_Handle_Input();
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
			PLAYER_Move(player, scale_v2(rot_v2(vector2(0, 1), -(player->facing)), player->speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_DOWN])
		{
			PLAYER_Move(player, scale_v2(rot_v2(vector2(0, -1), -(player->facing)), player->speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_RIGHT])
		{
			if(keystate[SDLK_LALT])
			{
				PLAYER_Move(player, scale_v2(rot_v2(vector2(1, 0), -(player->facing)), player->speed * engine_delta_time()));
			}
			else
			{
				PLAYER_Turn(player, player->turn_speed * engine_delta_time());
			}
		}
		
		if(keystate[SDLK_LEFT])
		{
			if(keystate[SDLK_LALT])
			{
				PLAYER_Move(player, scale_v2(rot_v2(vector2(-1, 0), -(player->facing)), player->speed * engine_delta_time()));
			}
			else
			{
				PLAYER_Turn(player, -player->turn_speed * engine_delta_time());
			}
		}
		
		if(keystate[SDLK_PAGEUP])
		{
			player->pos_height += player->speed * engine_delta_time();
		}
		
		if(keystate[SDLK_PAGEDOWN])
		{
			player->pos_height -= player->speed * engine_delta_time();
		}
			
		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				engine.running = false;
			}
			else if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{
					case '`':
					case '\'':
					case 'q':
						set_console_open(!is_console_open());
						break;


					case 'i':
						show_fps = !show_fps;
						break;

					case 'm':
						show_map = !show_map;
						break;

					case 'p':
						if(engine.edit_mode == 1 && show_texture_select == 0) engine.game_mode = !engine.game_mode;
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
}*/