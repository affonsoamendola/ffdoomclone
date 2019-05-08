
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"
#include "world.h"

#include "console.h"
#include "console_commands.h"

bool console_open = false;

char** console_history;

int console_cursor_location = 0;

void set_console_open(bool value)
{
	console_open = value;
}

bool get_console_open()
{
	return console_open;
}

char* get_console_history(int history_index)
{
	if(history_index >= 0 && history_index < HISTORY_SIZE)
	{
		return *(console_history + history_index);
	}
	else
	{
		return 0;
	}
}

void CONSOLE_scroll(int lines)
{
	console_cursor_location = 0;

	for(int i = 0; i < lines; i++)
	{
		free(*(console_history + HISTORY_SIZE - 1));

		for(int j = HISTORY_SIZE - 1; j > 0; j--)
		{
			*(console_history + j) = *(console_history + j - 1);
		}

		*(console_history) = malloc(CONSOLE_CHAR_LIMIT);

		for(int j = 0; j < CONSOLE_CHAR_LIMIT; j++)
		{
			*(*(console_history)+j) = '\0';
		}
	}
}

void CONSOLE_print(char* text)
{
	int location_pointer = 0;

	printf("%s\0", text);

	while(*(text + location_pointer) != '\0')
	{
		if(*(text + location_pointer) >= 32 && *(text + location_pointer) <= 126)
		{
			*(*(console_history) + console_cursor_location) = *(text + location_pointer); 
			console_cursor_location += 1;
		}
		
		if(*(text + location_pointer) == '\n')
		{
			CONSOLE_scroll(1);
		}

		location_pointer += 1;
	}
}

void CONSOLE_Init()
{
	console_history = malloc(HISTORY_SIZE * sizeof(char*));

	for(int i = 0; i < HISTORY_SIZE; i ++)
	{
		*(console_history + i) = malloc(CONSOLE_CHAR_LIMIT * sizeof(char));

		for(int j = 0; j < CONSOLE_CHAR_LIMIT; j++)
		{
			*(*(console_history + i) + j) = '\0';
		}
	}
}

void CONSOLE_Quit()
{
	for(int i = 0; i < HISTORY_SIZE; i ++)
	{
		free(console_history[i]);
	}

	free(console_history);
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

void parse_token(char** token)
{
	if(command_check("ver", token, 0))
	{

		COMMAND_ver();
	}

	if(command_check("intro", token, 0))
	{

		COMMAND_intro();
	}

	if(command_check("loadlevel", token, 0))
	{
		level_load(get_token_value(token, 1));
	}

	if(command_check("sector", token, 0))
	{
		if(command_check("show", token, 1))
		{
			COMMAND_sector_show(atoi(get_token_value(token, 2)));
		}
	}

	if(command_check("vertex", token, 0))
	{
		if(command_check("list", token, 1))
		{
			COMMAND_vertex_list();
		}
	}

	if(command_check("noclip", token, 0))
	{
		COMMAND_noclip();
	}
}

void parse_console(char* text_input)
{
	char** token;
	
	int parser_location = 0;
	int token_location = 0;

	int current_token = 0;

	token = malloc(sizeof(char*)*8);

	for(int i = 0; i < 8; i++)
	{
		*(token + i) = malloc(sizeof(char)*32);
		
		for(int j = 0; j < 32; j ++)
		{
			*(*(token + i) + j) = '\0';
		}
	}

	while(*(text_input + parser_location) == ' ')
	{
		parser_location ++;
	}
	
	while(*(text_input+parser_location) != '\0')
	{
		if(*(text_input+parser_location) == ' ')
		{
			current_token++;
			token_location = 0;

			while(*(text_input+parser_location) == ' ')
			{
				parser_location++;
			}

			if(*(text_input+parser_location) == '\0')
			{
				break;
			}
		}

		*(*(token + current_token) + token_location) = *(text_input + parser_location);

		token_location++;
		parser_location++;
	}

	parse_token(token);
}






