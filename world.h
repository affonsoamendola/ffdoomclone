#ifndef WORLD_H
#define WORLD_H

#include "vector2.h"

typedef struct SECTOR_
{
	int v_num;
	int * v;
}
SECTOR;

typedef struct LEVEL_
{
	int v_num;
	VECTOR2 * vertexes;
	int s_num;
	SECTOR * sectors;
}
LEVEL;

void WORLD_Init();

void level_load(const char * file_location);

#endif