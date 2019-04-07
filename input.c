#include <stdio.h>
#include <stdlib.h>

#include "engine.h"
#include "SDL.h"

#include "input.h"
#include "console.h"

extern bool e_running;

char* console_buffer;
int console_buffer_cursor_pos;

SDL_Event event;

char lower_case_symbols[20] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '[', ']', ';', '\'', ',', '.', '/'};
char upper_case_symbols[20] = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', ':', '\"', '<', '>', '?'};

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
				*(console_buffer + console_buffer_cursor_pos - 1) = ' ';
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
	if(get_console_open())
	{
		INPUT_Handle_Console();
	}
	else
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