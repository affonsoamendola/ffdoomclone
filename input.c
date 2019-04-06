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

void INPUT_Handle()
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
				printf("OPENING CONSOLE\n");
				set_console_open(!get_console_open());
			}
			else if(get_console_open() == true)
			{
				if(event.key.keysym.sym == SDLK_BACKSPACE && console_buffer_cursor_pos > 0)
				{
					*(console_buffer + console_buffer_cursor_pos - 1) = ' ';
					console_buffer_cursor_pos -= 1;
				}
				else if(event.key.keysym.sym >= 32 && event.key.keysym.sym <= 126)
				{
					*(console_buffer + console_buffer_cursor_pos) = event.key.keysym.sym;
					console_buffer_cursor_pos += 1;
				}

				if(event.key.keysym.sym == SDLK_RETURN)
				{
					parse_console(console_buffer);
					CONSOLE_scroll(1);
					CONSOLE_print(console_buffer);
					console_buffer_cursor_pos = 0;
				}
			}
		}
	}
}

void INPUT_Init()
{
	console_buffer = malloc(CONSOLE_CHAR_LIMIT*sizeof(char));
	console_buffer_cursor_pos = 0;
}