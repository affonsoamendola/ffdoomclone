#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>


SDL_Surface* screen = NULL;

int main(int argc, char** argv)
{
	ENGINE_Init();
	
	ENGINE_Quit();
}

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
}

void ENGINE_Quit()
{
	printf("Quitting SDL...\n");

	SDL_Quit();
	exit(0);
}