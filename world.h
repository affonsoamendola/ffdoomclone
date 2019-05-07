#ifndef WORLD_H
#define WORLD_H

#include "vector2.h"
#include "gfx.h"

typedef struct EDGE_
{
	int v_start;
	int v_end;

	GFX_TEXTURE_PARAM text_param;

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

	GFX_TEXTURE_PARAM text_param_ceil;
	GFX_TEXTURE_PARAM text_param_floor;

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

EDGE * get_edge_from_level(int sector_index, int edge_index);
EDGE * get_edge_at(SECTOR * sector, int edge_index);

VECTOR2 get_vertex_from_sector(SECTOR * sector, int edge_index, int start_or_end);
void get_closest_vertex(VECTOR2 pos, VECTOR2 * closest, int * vertex_index, float * distance);
void get_closest_edge(VECTOR2 pos, EDGE ** edge, VECTOR2 * projection, int * edge_index, int * sector_index, float * distance);

int WORLD_add_vertex(VECTOR2 vertex);
int WORLD_add_edge_to_sector(SECTOR * sector, int vertex_start_index, int vertex_end_index);
int WORLD_add_sector_to_level(SECTOR * sector);

void WORLD_delete_vertex_at(int index);
void WORLD_delete_sector_at(int index);

void WORLD_remove_n_vertexes(int n);

#endif