
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "console.h"

bool console_open = false;

char** console_history;

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

void CONSOLE_Init()
{
	console_history = malloc(HISTORY_SIZE * sizeof(char*));

	for(int i = 0; i < HISTORY_SIZE; i ++)
	{
		*(console_history + i) = malloc(CONSOLE_CHAR_LIMIT * sizeof(char));
	}
}

void parse_console(char* text_input)
{
	char* token;
	int parser_location = 0;
	int token_location = 0;

	token = malloc(sizeof(char)*256);

	while(*(text_input + parser_location) != 0)
	{
		while(*(text_input + parser_location) != ' ' && *(text_input+parser_location) != 0)
		{
			*(token + token_location) = *(text_input + parser_location);
			token_location++;
			parser_location++;
		}

		token_location = 0;

		parse_token(token);
	}
}

void parse_token(char * token)
{
	if(token == "ver")
	{
		CONSOLE_print("Fofonso's Raytracing Engine\n");
		CONSOLE_print("Codename RT76800\n");
		CONSOLE_print("Version ");
		CONSOLE_print(ENGINE_version());
		CONSOLE_print("\n");
	}
}




