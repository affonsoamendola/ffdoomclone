#ifndef ENGINE_H
#define ENGINE_H

#define true 1
#define false 0

#define ENGINE_VERSION "0.01"
#define PIXEL_SCALE 3

typedef int bool;

void ENGINE_Init();
void ENGINE_Quit();

void ENGINE_Loop();

char* ENGINE_version();

#endif