#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"

#include "input.h"
#include "console.h"
#include "console_commands.h"
#include "gfx.h"
#include "world.h"
#include "time.h"
#include "list.h"

#include "engine.h"

SDL_Surface* screen = NULL;
bool e_running = false;

clock_t current_frame_start;
clock_t last_frame_end;

float current_fps = 0;

void ENGINE_Init()
{
	printf("Initting SDL...\n");
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)==-1))
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		exit(-1);
	}
	printf("SDL Initted.\n");

	printf("Initting SDL_Image...\n");
	if((IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)==-1))
	{
		printf("Could not initialize SDL_Image: %s\n", SDL_GetError());
		exit(-1);
	}
	printf("SDL_Image Initted.\n");

	screen = SDL_SetVideoMode(SCREEN_RES_X * PIXEL_SCALE, SCREEN_RES_Y * PIXEL_SCALE, 32, SDL_SWSURFACE);

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

	SDL_FreeSurface(screen);

	CONSOLE_Quit();
	INPUT_Quit();
	GFX_Quit();
	IMG_Quit();
	SDL_Quit();
	exit(0);
}

float ENGINE_delta_time()
{
	clock_t current_time;

	current_time = clock();

	return (float)(current_time - current_frame_start) / (float)CLOCKS_PER_SEC;
}

void ENGINE_Loop()
{
	
	current_frame_start = clock();

	GFX_Render();
	INPUT_Handle();

	current_fps = (float)CLOCKS_PER_SEC / (float)(clock() - current_frame_start);
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
