#include <stdio.h>
#include <stdlib.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "input.h"
#include "console.h"
#include "console_commands.h"
#include "gfx.h"
#include "world.h"
#include "game.h"
#include "time.h"

#include "editor.h"

#include "engine.h"

#include "ff_linked_list.h"

Engine engine;

//Initializes engine structure;
void engine_init()
{
	//Default values
	engine.is_running = true;
	engine.debug_flag = true;

	engine.game_mode = MODE_GAME;

	engine.delta_time = 0.0f;
  	engine.fps_samples = 0.0f;
    engine.fps_samples_num = 0.0f;
    engine.performance_freq = SDL_GetPerformanceFrequency();

    engine.show_fps = false;

	//Initializes basic systems
	printf("Initting SDL2...\n");
	if((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)==-1))
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		exit(-1);
	}

	printf("Initting SDL_Image...\n");
	if((IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)==-1))
	{
		printf("Could not initialize SDL_Image: %s\n", SDL_GetError());
		exit(-1);
	}

	init_console();
	printf_console("Console Initted.\n");
	
	printf_console("Initting Input...\n");
	init_input();
	printf_console("Input Initted.\n");

	printf_console("Initting GFX...\n");
	GFX_init();
	printf_console("GFX Initted.\n");

	printf_console("Initting World...\n");
	init_world();
	printf_console("World Initted.\n");

	printf_console("Initting Editor...\n");
	init_editor();
	printf_console("Editor Initted.\n");

	printf_console("Initting Game...\n");
	init_game();
	printf_console("Game Initted.\n");

	command_intro(NULL);
}

void engine_quit()
{
	quit_game();
	quit_world();
	GFX_quit();
	IMG_Quit();
	SDL_Quit();
	exit(0);
}

void engine_change_mode(uint32_t mode)
{
	if(mode == engine.game_mode) return;

	if(mode == MODE_EDITOR)
	{
		engine.game_mode = mode;
		enter_editor();
	}

	if(mode == MODE_GAME)
	{
		engine.game_mode = mode;
		leave_editor();
	}
}

void signal_quit(void* engine)
{	
	
	((Engine*)engine)->is_running = false;
}

void engine_loop()
{
	const Uint64 frame_start = SDL_GetPerformanceCounter();

	update_input();
	if(engine.game_mode == MODE_EDITOR)
	{
		update_editor();
	}
	else if(engine.game_mode == MODE_GAME)
	{
		update_game();
	}

	GFX_render_start();

	if(engine.game_mode == MODE_EDITOR)
	{
		draw_editor();
	}
	else if(engine.game_mode == MODE_GAME)
	{
		draw_game();
	}

	if(console.open) draw_console();

	GFX_draw_string_color_f(point2(250, 1), 3, DEBUG_TEXT_COLOR, "    FPS: %-.1f", engine_fps());
	GFX_draw_string_color_f(point2(250, 7), 3, DEBUG_TEXT_COLOR, "Avg FPS: %-.1f", engine_average_fps());
	GFX_render_end();

	//Stops the timer.
    const Uint64 frame_end = SDL_GetPerformanceCounter();
    
    //Gets frame delta time in seconds.
    engine.delta_time = ( frame_end - frame_start ) / (double)(engine.performance_freq);
   	//Adds a sample of the FPS for the average calculation
    engine.fps_samples += 1.0/engine.delta_time;
    //Adds increments the amount of samples.
    engine.fps_samples_num += 1.0f;
    //If the samples get to a high number, cut down on the value.
    //I believe this will drift the results after a while, so
    //I'm not completely sure of the numerical stability of this.
    //TODO: Test numerical stability of this.
    if(engine.fps_samples_num >= 10000.0) 
    {
    	engine.fps_samples /= 10000.0;
    	engine.fps_samples_num /= 10000.0;
    }
}   

int main(int argc, char** argv)
{
	engine_init();

	while(engine.is_running == true)
	{
		engine_loop();
	}

	engine_quit();
}
