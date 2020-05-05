#ifndef WORLD_H
#define WORLD_H

#include "ff_vector2.h"
#include "gfx.h"

/*
#define COLLIDED 1
#define NO_COLLISION 0
#define NO_COLLISION_SECTOR_CHANGE -1
*/

typedef struct Sector_ Sector;
typedef struct Edge_ Edge;

typedef struct Vertex_ 
{
	Vector2f value;
	//float height; //Eh maybe in the future. SLOPES MAN SLOPES
} Vertex;

typedef struct Edge_
{
	Vertex* vertex_start;
	Vertex* vertex_end;

	TextureParam texture_param;

	bool is_portal;

	Sector* neighboring_sectors[2];
} Edge;

typedef struct Sector_
{
	uint32_t sector_id;

	float ceiling_height;
	float floor_height;

	Color sector_tint;

	TextureParam ceiling_texture_param;
	TextureParam floor_texture_param;

	uint32_t edge_size;
	Edge** edges;
} Sector;

typedef struct Entity_
{
} Entity;

typedef struct World_
{
	uint32_t edge_size;
	Edge* edges;

	uint32_t vertex_size;
	Vertex* vertexes;

	uint32_t sector_size;
	Sector* sectors;

	uint32_t entities_size;
	Entity* entities;
} World;

extern World world;

void init_world();
void quit_world();


/*
void WORLD_Update();

void level_load(const char * file_location);
VECTOR2 get_vertex_at(int index);

EDGE * get_edge_from_level(int sector_index, int edge_index);
EDGE * get_edge_at(SECTOR * sector, int edge_index);
SECTOR * get_sector_at(int sector_index);

VECTOR2 get_vertex_from_sector(SECTOR * sector, int edge_index, int start_or_end);
void get_closest_vertex(VECTOR2 pos, VECTOR2 * closest, int * vertex_index, float * distance);
void get_closest_edge(VECTOR2 pos, EDGE ** edge, VECTOR2 * projection, int * edge_index, int * sector_index, float * distance);

int WORLD_add_vertex(VECTOR2 vertex);
int WORLD_add_edge_to_sector(SECTOR * sector, int vertex_start_index, int vertex_end_index);
int WORLD_add_sector_to_level(SECTOR * sector);

void WORLD_delete_vertex_at(int index);
void WORLD_delete_sector_at(int index);

void WORLD_remove_n_vertexes(int n);

int WORLD_Check_Collision(	int start_sector, VECTOR2 start_pos, VECTOR2 move_amount, VECTOR2 * intersected_position, int * end_sector, 
							int check_knees, float pos_height, float height, float knee_height);

VECTOR2 convert_ss_to_ws(CAMERA * camera, POINT2 screen_space, float height);
VECTOR2 convert_ss_to_rs(CAMERA * camera, POINT2 screen_space, float height);

VECTOR2 convert_rs_to_ws(VECTOR2 relative_space);
POINT2 convert_ws_to_ss(CAMERA * camera, VECTOR2 world_space, float height);

void convert_ws_to_rs(	VECTOR2 world_space, float world_height,
						VECTOR2 * relative_space, float * relative_height);
POINT2 convert_rs_to_ss(CAMERA * camera, VECTOR2 relative_space, float relative_height);
*/
#endif