#ifndef WORLD_H
#define WORLD_H

#include "vector2.h"



typedef struct EDGE_
{
	int v_start;
	int v_end;

	int is_portal;
	int neighbor_sector_id;
}
EDGE;

typedef struct SECTOR_
{
	int sector_id;
	int e_num;
	float ceiling_height;
	float floor_height;
	EDGE * e;
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
VECTOR2 get_vertex_at(int index);

EDGE * get_edge_at(SECTOR * sector, int edge_index);

VECTOR2 get_vertex_from_sector(SECTOR * sector, int edge_index, int start_or_end);


#endif