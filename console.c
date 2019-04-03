
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
}






