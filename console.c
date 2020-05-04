#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "world.h"
#include "player.h"

#include "console.h"
#include "console_commands.h"
/*
extern bool edit_mode;
extern PLAYER * player;
/*
Console console;

void set_console_open(bool value)
{

	console.open = value;
}

bool is_console_open()
{

	return console.open;
}

char* get_console_history(int history_index)
{
	if(history_index >= 0 && history_index < CONSOLE_HISTORY_SIZE)
	{
		return console.history + history_index * CONSOLE_CHAR_LIMIT;
	}
	else
	{
		return NULL;
	}
}

void scroll_console(int lines)
{
	console.cursor_location = 0;

	char buffer[CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1)];

	for(int i = 0; i < lines; i ++)
	{
		memcpy(buffer, console.history, CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1));
		memcpy(console.history + CONSOLE_CHAR_LIMIT, buffer, CONSOLE_CHAR_LIMIT * (CONSOLE_HISTORY_SIZE - 1));
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
			*(console.history + cursor_location) = character; 
			cursor_location++;
		}
	}
}

Console* console_init()
{
	console.open = false;
	console.cursor_location = 0;
	memset(console.history, '\0', CONSOLE_CHAR_LIMIT * CONSOLE_HISTORY_SIZE);

	return &console;
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

#define COMMANDS_AMOUNT 11
ConsoleCommand console_commands[COMMANDS_AMOUNT] = 
{
	{"ver", 0, COMMAND_ver},
	{"intro", 0, COMMAND_intro},
	{"help", 1, COMMAND_help},
	{"sector_show", 1, COMMAND_sector_show},
	{"vertex_list", 0, COMMAND_vertex_list},
	{"noclip", 0, COMMAND_vertex_list},
	{"set_tint", 3, COMMAND_set_tint},
	{"set", 2, COMMAND_set},
	{"save_level", 1, COMMAND_save_level},
	{"load_level", 1, COMMAND_load_level},
	{"edit", 0, COMMAND_edit}
};

void parse_tokens(char** tokens, int args_num)
{
	ConsoleCommand command;
	for(int i = 0; i < COMMANDS_AMOUNT; i++)
	{
		if(strcmp(console_commands[i].command, tokens[0]) == 0)
		{
			if((args_num-1) >= console_commands[i].arg_num)
			{
				console_commands[i].function(tokens + 1);
				break;
			}
			else
			{
				printf_console(	"The %s command takes %d arguments.", 
								console_commands[i].command, 
								console_commands[i].arg_num);
			}
		}
	}
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
}*/