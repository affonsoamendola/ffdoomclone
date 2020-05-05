#ifndef ENGINE_H
#define ENGINE_H

#include "SDL2/SDL.h"
#include "ff_stb.h"

#define ENGINE_VERSION "0.1"

#define MODE_EDITOR 2
#define MODE_GAME 1

#define DEBUG_TEXT_COLOR color(255, 150, 0, 200)

typedef struct Engine_
{
	char* window_title;

	bool is_running;

	uint32_t game_mode;

	//Frame Timing Values.
	bool show_fps;
	//1000.0f / delta_time = FPS
	//Delta time is in Seconds.
	double delta_time;
	//FPS_Samples/FPS_Samples_Num = Average FPS
    double fps_samples;
    double fps_samples_num;
    double performance_freq;
} Engine;

extern Engine engine;

void engine_init();
void engine_quit();
void signal_quit(void* engine);

void engine_loop();

static inline double engine_average_fps() { return engine.fps_samples/engine.fps_samples_num; }
static inline double engine_fps() { return 1.0f/engine.delta_time; }
static inline double engine_delta_time() { return engine.delta_time; }
//int engine_blink_state();


#endif
