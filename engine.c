#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "input.h"
#include "console.h"
#include "gfx.h"

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

	screen = SDL_SetVideoMode(320, 240, 8, SDL_SWSURFACE);

	if(screen == NULL)
	{
		printf("Could not set up video mode 320x240x8: %s\n", SDL_GetError());
	}

	printf("Initting Console Subsystem...\n");

	CONSOLE_Init();

	INPUT_Init();

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
