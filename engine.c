#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "input.h"
#include "console.h"
#include "console_commands.h"
#include "gfx.h"
#include "world.h"

#include "engine.h"

SDL_Surface* screen = NULL;
bool e_running = false;

void ENGINE_Init()
{
	printf("Initting SDL...\n");
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)==-1))
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		exit(-1);
	}

	printf("SDL Initted.\n");

	screen = SDL_SetVideoMode(SCREEN_RES_X * PIXEL_SCALE, SCREEN_RES_Y * PIXEL_SCALE, 16, SDL_SWSURFACE);

	if(screen == NULL)
	{
		printf("Could not set up video mode 320x240x8: %s\n", SDL_GetError());
	}

	printf("Initting Console Subsystem...\n");

	CONSOLE_Init();

	GFX_Init();

	INPUT_Init();

	WORLD_Init();

	COMMAND_intro();

	e_running = true;
}

void ENGINE_Quit()
{
	printf("Quitting SDL...\n");

	SDL_Quit();
	exit(0);
}

void ENGINE_Loop()
{
	GFX_Render();
	INPUT_Handle();
}

int main(int argc, char** argv)
{
	ENGINE_Init();
	
	while(e_running == true)
	{
		ENGINE_Loop();
	}

	ENGINE_Quit();
}

char* ENGINE_version()
{
	char* text;

	text = malloc(sizeof(char)*16);

	text = ENGINE_VERSION;

	return text;
}
