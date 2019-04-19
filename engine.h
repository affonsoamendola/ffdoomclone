#ifndef ENGINE_H
#define ENGINE_H

#define true 1
#define false 0

#define EPSILON 1e-6
#define PI 3.14159

#define ENGINE_VERSION "0.02"
#define PIXEL_SCALE 3

#define SCREEN_RES_X 320
#define SCREEN_RES_Y 240

typedef int bool;

void ENGINE_Init();
void ENGINE_Quit();

void ENGINE_Loop();

char* ENGINE_version();

float ENGINE_delta_time();

#endif