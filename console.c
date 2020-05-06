#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "world.h"
#include "player.h"
#include "gfx.h"
#include "input.h"

#include "console.h"
#include "console_commands.h"

Console console;

//Actions allowed to happen while console is open
ActionCode console_enabled_actions[] =
{
	ACTION_QUIT,
	ACTION_TOGGLE_CONSOLE,
	ACTION_CONFIRM_CONSOLE,
	ACTION_MOUSE_MOVE,
	ACTION_MOUSE_DRAG_RIGHT,
	ACTION_SCROLL_WHEEL
};
const static uint32_t console_enabled_actions_size = sizeof(console_enabled_actions)/sizeof(ActionCode);

//Initializes all console state variables.
void init_console()
{
	console.open = false;

	memset(console.history, '\0', CONSOLE_CHAR_LIMIT * CONSOLE_HISTORY_SIZE);
	
	set_console_font(0);

	console.background_color = color(64, 35, 0, 170);
	console.entry_color = color(80, 50, 0, 170);
	console.history_text_color = color(128, 70, 40, 170);
	console.entry_text_color = color(200, 160, 100, 170);

	//Creates the rainbow effect holder for the console rainbow strip(tm)
	console.rainbow_effect = create_effect_rainbow(0.25, 200);
}

//Draws the console
void draw_console()
{
	//Updates the console rainbow effect, a rainbow effect doesnt do anything on update
	//So NULL for second argument
	update_effect(&console.rainbow_effect, NULL);

	//Gets the current installed font at the console font_ID
	Font font = GFX_get_font(console.font);	
	
	//Draws console background
	for(int i = 0; i < CONSOLE_HISTORY_SIZE; i++)
	{
		//Calculates the offset of each line in relation to the font height.
		Point2 offset = point2(0, (font.char_h + 1)*i); 
		
		//Gets whats the current background color is
		//If its the last line, gets entry color instead of background color
		Color back_color;
	
		if(i == CONSOLE_HISTORY_SIZE-1) back_color = console.entry_color;
		else back_color = console.background_color;

		

		//Fills the background rectangle for each line.
		GFX_fill_rectangle(	offset_rect(rect(0, 0, gfx.screen_res_x, font.char_h+1),
										offset.x, offset.y),
							back_color);
	}

	//Draws the rainbow strip at the bottom of the console
	Color rainbow_strip_color;
	for(int i = 0; i < gfx.screen_res_x; i++)
	{
		//temp is the angle offset of the effect, 
		float temp = (i*2.0*M_PI)/gfx.screen_res_x; 
		//gets the current state of the effect offset by temp and puts it in rainbow_strip_color
		get_effect(&console.rainbow_effect, &temp, &rainbow_strip_color);
		//Draw current pixel of the rainbow strip
		GFX_set_pixel(i, (font.char_h+1)*CONSOLE_HISTORY_SIZE, rainbow_strip_color);
	}

	//Flush pixels to GPU (And screen)
	GFX_update_pixels();

	//Draws console text
	for(int i = 0; i < CONSOLE_HISTORY_SIZE; i++)
	{
		//Calculates the offset of each line in relation to the font height.
		Point2 offset = point2(0, (font.char_h+1)*i); 

		Color text_color;

		if(i == CONSOLE_HISTORY_SIZE-1) text_color = console.entry_text_color;
		else text_color = console.history_text_color;

		//Draws the actual Console text history
		GFX_draw_string_color( offset, console.font, get_console_history(CONSOLE_HISTORY_SIZE-1-i), 
							   text_color);
	}
}

void set_console_font(uint32_t font)
{
	
	console.font = font;
}

void toggle_console(void * console_)
{
	Console* console = (Console*)console_;
	set_console_open(!console->open);
}

void set_console_open(bool value)
{
	if(value)
	{
		set_enabled_actions(console_enabled_actions, console_enabled_actions_size);
		start_text_input(console.history[0], CONSOLE_CHAR_LIMIT);
		console.open = true;
	}
	else
	{
		set_enabled_actions(NULL, 0);
		end_text_input();
		console.open = false;
	}
}

bool is_console_open()
{

	return console.open;
}

char* get_console_history(int history_index)
{
	if(history_index >= 0 && history_index < CONSOLE_HISTORY_SIZE)
	{
		return (char*)(console.history + history_index);
	}
	else
	{
		return NULL;
	}
}

void scroll_console(int lines)
{
	char buffer[CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1)];

	for(int i = 0; i < lines; i ++)
	{
		memcpy(buffer, console.history, CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1));
		memcpy(console.history + 1, buffer, CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1));
		memset(console.history, '\0', CONSOLE_CHAR_LIMIT);
	}
}

void printf_console(const char * char_string, ...)
{
	char buffer[CONSOLE_CHAR_LIMIT] = {0};

	va_list args;
	va_start(args, char_string);
	
	vsnprintf(buffer, CONSOLE_CHAR_LIMIT, char_string, args);

	va_end(args);

	int cursor_location = 0;

	for(int i = 0; i < CONSOLE_CHAR_LIMIT && buffer[i] != '\0'; i++)
	{
		char character = buffer[i];

		if(character == '\n') 
		{
			scroll_console(1);
			cursor_location = 0;
		}
		else
		{			
			*(console.history[0] + cursor_location) = character; 
			cursor_location++;
		}
	}
}

bool command_check(char * command, char** token, int token_number)
{
	if(strcmp(command, *(token + token_number)) == 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}

char* get_token_value(char** token, int index)
{

	return *(token + index);
}

ConsoleCommand console_commands[] = 
{
	{"ver", 0, command_ver},
	{"intro", 0, command_intro},
	{"help", 1, command_help},
	{"vertex", 1, command_vertex},
	{"vertexlist", 1, command_vertex_list},
	{"delvertex", 1, command_remove_vertex},
/*	{"sector_show", 1, COMMAND_sector_show},
	{"vertex_list", 0, COMMAND_vertex_list},
	{"noclip", 0, COMMAND_vertex_list},
	{"set_tint", 3, COMMAND_set_tint},
	{"set", 2, COMMAND_set},
//	{"save_level", 1, COMMAND_save_level},
//	{"load_level", 1, COMMAND_load_level},
//	{"edit", 0, COMMAND_edit}*/
};
static uint32_t console_commands_size = sizeof(console_commands)/sizeof(ConsoleCommand);

void parse_tokens(char** tokens, int args_num)
{
	ConsoleCommand command;
	for(int i = 0; i < console_commands_size; i++)
	{
		if(strcmp(console_commands[i].command, tokens[0]) == 0)
		{
			if((args_num-1) >= console_commands[i].arg_num)
			{
				console_commands[i].function(tokens + 1);
				return;
			}
			else
			{
				printf_console(	"The %s command takes %d arguments.", 
								console_commands[i].command, 
								console_commands[i].arg_num);
			}
		}
	}
	scroll_console(1);
}

#define MAX_ARGSIZE 32
void parse_console(const char* text_input)
{	
	char** tokens;

	int text_location = 0;
	int args_num = 0;

	tokens = malloc(1 * sizeof(char*));

	while(text_location < CONSOLE_CHAR_LIMIT)
	{
		while(text_input[text_location] == ' ')
		{
			text_location++;
		}

		if(text_input[text_location] == '\0') break;

		args_num++;
		tokens = realloc(tokens, args_num * sizeof(char*));
		tokens[args_num-1] = malloc(MAX_ARGSIZE * sizeof(char));
		memset(tokens[args_num-1], '\0', MAX_ARGSIZE);

		for(int parser_location = 0; 
			
			parser_location < CONSOLE_CHAR_LIMIT &&
			text_input[text_location] != '\0' &&
			text_input[text_location] != ' '; 

			parser_location++)
		{
			if(parser_location < MAX_ARGSIZE-1) 
			{
				tokens[args_num-1][parser_location] = text_input[text_location];
			}
			
			text_location++;
		}
	}

	parse_tokens(tokens, args_num);

	for(int i = 0; i < args_num; i++)
	{
		free(tokens[i]);
	}
	free(tokens);
}

void enter_console(void* console_cursor_location)
{
	*(uint32_t*)console_cursor_location = 0;
	parse_console(get_console_history(0));
}