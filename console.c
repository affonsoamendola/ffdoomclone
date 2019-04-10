
#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "console.h"

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

		*(console_history) = malloc(CONSOLE_CHAR_LIMIT * sizeof(char));

		for(int j = 0; j < CONSOLE_CHAR_LIMIT; j++)
		{
			*(*(console_history)+j) = '\0';
		}
	}
}

void CONSOLE_print(char* text)
{
	int location_pointer = 0;

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

void CONSOLE_command_ver()
{
	CONSOLE_print("\nRT76800");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
}

void CONSOLE_command_intro()
{
	CONSOLE_print("\nFofonso's Raytracing Engine\n");
	CONSOLE_print("\nCodename RT76800");
	CONSOLE_print("\nCopyright Affonso Amendola, 2019");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
	CONSOLE_print("\n\nBe Excellent to Each Other");
	CONSOLE_print("\n----------------------------------------");
}

int str_compare(char * str_a, char * str_b)
{
	int counter = 0;

	while(*(str_a + counter) != '\0')
	{	
		if(*(str_a + counter) != *(str_b + counter))
		{
			return 0;
		}

		counter++;
	}

	return 1;
}

void parse_token(char * token)
{
	printf("TOKEN:%s\n", token);

	if(str_compare("ver", token))
	{
		CONSOLE_command_ver();
	}

	if(str_compare("intro", token))
	{
		CONSOLE_command_intro();
	}
}

void parse_console(char* text_input)
{
	char* token;
	int parser_location = 0;
	int token_location = 0;

	token = malloc(sizeof(char)*256);

	for(int i = 0; i < 256; i ++)
	{
		*(token + i) = '\0';
	}

	while(*(text_input + parser_location) == ' ')
	{
		parser_location ++;
	}
	
	while(*(text_input + parser_location) != ' ' && *(text_input+parser_location) != '\0')
	{
		*(token + token_location) = *(text_input + parser_location);
		token_location++;
		parser_location++;
	}

	token_location = 0;
	parse_token(token);
}






