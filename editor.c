#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "editor.h"

#include "ff_vector2.h"
#include "gfx.h"
#include "engine.h"
#include "effect.h"
#include "input.h"

Editor editor;

//Relates an Action to a function (And its data)
Action editor_actions[] =
{
	{ACTION_QUIT, signal_quit, &engine},
	{ACTION_MOUSE_MOVE, (ActionFunction)move_editor_cursor, &editor.cursor_location},
	{ACTION_MOUSE_DRAG_RIGHT, (ActionFunction)move_editor_view, &editor.center},
	{ACTION_SCROLL_WHEEL, (ActionFunction)scroll_wheel_zoom, &editor.zoom}
};
const static uint32_t editor_actions_size = sizeof(editor_actions)/sizeof(Action);

//Initializes state.
void init_editor()
{
	printf("Initting Editor\n");

	editor.cursor_location = ZERO_VECTOR2F;
	editor.center = ZERO_VECTOR2F;

	editor.cursor_color = COLOR_WHITE;
	editor.current_cursor_color = COLOR_WHITE;
	editor.cursor_effect = create_effect_breathe(0.7, 1.0, 4.0);

	editor.zoom = 100.0f;
	editor.zoom_speed = 10.0f;

	editor.grid_size = 0.1f;
	editor.grid_color = color(75, 75, 75, 255);

	set_input_actions(editor_actions, editor_actions_size);
}

//Ran once per frame, updates state.
void update_editor()
{
	float breathe = 0.0f;

	update_effect(&editor.cursor_effect, &breathe);
	editor.current_cursor_color = scale_f_color(editor.cursor_color, breathe);

	get_editor_bounds(&editor.viewport_top_left, &editor.viewport_bottom_right);
	editor.viewport_size_x = fabs(editor.viewport_bottom_right.x - editor.viewport_top_left.x); 
	editor.viewport_size_y = fabs(editor.viewport_bottom_right.y - editor.viewport_top_left.y); 
}

void scroll_wheel_zoom(void* zoom, int direction)
{
	printf("SCROLL %d\n", direction);
	*(float*)zoom += editor.zoom_speed*direction;
}

//Draws all of the text on the editor
void draw_text_overlay()
{
	GFX_draw_string_color_f(point2(4, 220), 3, DEBUG_TEXT_COLOR, "mx= %+-4.2f my= %+-4.2f", editor.cursor_location.x, editor.cursor_location.y);
}

//Main draw function for the editor.
void draw_editor()
{
	draw_grid();
	draw_editor_cursor(editor.cursor_location, editor.current_cursor_color);
	GFX_update_pixels(); // Flushes pixel data.
	draw_text_overlay();
}	

//Draws Grid
void draw_grid()
{
	int grid_points_x = editor.viewport_size_x/editor.grid_size;
	int grid_points_y = editor.viewport_size_y/editor.grid_size;

	Vector2f top_left_grid = get_closest_grid(sum_v2(editor.viewport_top_left, vector2f(editor.grid_size/2., -editor.grid_size/2.)), editor.grid_size);

	for(int x = 0; x <= grid_points_x; x++)
	{
		for(int y = 0; y <= grid_points_y; y++)
		{
			Point2 screen_pos = editor_ws_to_ss(sum_v2(top_left_grid, vector2f(x*editor.grid_size, -y*editor.grid_size)),
												editor.center,
												editor.zoom);

			GFX_set_pixel_clipped(screen_pos.x, screen_pos.y, editor.grid_color);
		}
	}
}

//Draws cursor
void draw_editor_cursor(const Vector2f location, const Color color)
{
	Point2 cursor_screen_pos = editor_ws_to_ss(location, editor.center, editor.zoom);

	for(int i = -5; i <= 5; i++)
	{
		if(i < -1 || i > +1)
		{
			GFX_set_pixel_clipped(	cursor_screen_pos.x + i, cursor_screen_pos.y, 
									color);
		}
	}

	for(int j = -5; j <= 5; j++)
	{
		if(j < -1 || j > +1)
		{
			GFX_set_pixel_clipped(	cursor_screen_pos.x, cursor_screen_pos.y + j, 
									color);
		}
	}
}

void clip_cursor(const Vector2f top_left, const Vector2f bot_right)
{
	if(editor.cursor_location.x < top_left.x) editor.cursor_location.x = top_left.x;
	if(editor.cursor_location.y > top_left.y) editor.cursor_location.y = top_left.y;
	if(editor.cursor_location.x > bot_right.x) editor.cursor_location.x = bot_right.x;
	if(editor.cursor_location.y < bot_right.y) editor.cursor_location.y = bot_right.y;
}

//Moves cursor, but only if mouse right isnt held (In that case there is a dragging going on)
void move_editor_cursor(void* editor_cursor_, Vector2f amount)
{
	if(!is_mouse_held(INPUT_MOUSE_RIGHT))
	{
		Vector2f* editor_cursor = (Vector2f*)(editor_cursor_);
		*editor_cursor = sum_v2(*editor_cursor, scale_v2(amount, 1.0/editor.zoom));
		clip_cursor(editor.viewport_top_left, editor.viewport_bottom_right);
	}
}

//Moves editor view.
void move_editor_view(void* editor_center, Vector2f amount)
{
	*(Vector2f*)editor_center = sum_v2(*(Vector2f*)editor_center, scale_v2(amount, 1.0/editor.zoom));
}

/*
#define DEFAULT_FLOOR_HEIGHT 0.0f
#define DEFAULT_CEIL_HEIGHT 1.0f

#define EDIT_TEXTURE_CEIL 0
#define EDIT_TEXTURE_FLOOR 1
#define EDIT_TEXTURE_EDGE 2

VECTOR2 editor_center;
VECTOR2 editor_cursor;

int closest_vector_index;
int closest_edge_index;
int closest_sector_index;
VECTOR2 closest_edge_projection;

VECTOR2 closest_vector;
EDGE * closest_edge;
SECTOR * closest_sector;

float closest_vector_distance;
float closest_edge_distance;

float grid_size = 0.1;

float editor_zoom = 50.;

float cursor_speed = 1.f;
#define crawl_cursor_speed 1.5f;
#define walk_cursor_speed 3.f;
#define run_cursor_speed 6.f;

#define cursor_color GFX_Color(255, 0, 0)

extern LEVEL loaded_level; 
extern GFX_TEXTURE loaded_textures[TEX_ID_SIZE];
extern SDL_Surface * screen;

extern float current_fps;
extern bool show_fps;

bool show_help = 1;

extern bool edit_mode;
extern bool game_mode;

int grabbed = 0;
int snap_to_grid = 0;
int occupied = 0;
int show_info = 0;

int show_texture_select = 0;
POINT2 selected_texture = (POINT2){0, 0};
GFX_TEXTURE_PARAM selected_texture_param;
int selecting_texture_for = 2; //0 = ceiling, 1 = floor, 2 = edge

int grabbed_vector_index;
VECTOR2 grabbed_vector_start;
VECTOR2 grabbed_vector_location;

char buffer[128];

VECTOR2 top_left_border;
VECTOR2 bottom_right_border;

int new_vector_start_index;
VECTOR2 new_vector_start;

int first_vector_index;
VECTOR2 first_vector;

SECTOR * creating_sector;

int new_sector_size = 0;

int drawing_sector = 0;

extern PLAYER * player;



POINT2 convert_ws_to_editor_ss(VECTOR2 pos)
{
	POINT2 editor_ss;

	editor_ss.x = (pos.x - editor_center.x)*editor_zoom + engine.gfx->screen_res_x/2;
	editor_ss.y = engine.gfx->screen_res_y/2 - (pos.y - editor_center.y)*editor_zoom;

	return editor_ss;
}

VECTOR2 convert_editor_ss_to_ws(POINT2 pos)
{
	VECTOR2 ws;

	ws.x = (pos.x - engine.gfx->screen_res_x/2)/editor_zoom + editor_center.x;
	ws.y = -((pos.y - engine.gfx->screen_res_y/2)/editor_zoom - editor_center.y);

	return ws;
}

void save_sector()
{

	WORLD_add_sector_to_level(creating_sector);
}

void new_sector()
{
	drawing_sector = 1;
	occupied = 1;

	creating_sector = malloc(sizeof(SECTOR));
	creating_sector->sector_id = loaded_level.s_num;

	creating_sector->e_num = 0;

	creating_sector->floor_height = DEFAULT_FLOOR_HEIGHT;
	creating_sector->ceiling_height = DEFAULT_CEIL_HEIGHT;

	creating_sector->text_param_ceil = DEFAULT_TEXTURE_PARAM;
	creating_sector->text_param_floor = DEFAULT_TEXTURE_PARAM;

	creating_sector->e = NULL;

	if(closest_vector_distance >= 0.1f)
	{
		if(snap_to_grid)
			new_vector_start = get_closest_grid(editor_cursor);
		else
			new_vector_start = editor_cursor;

		new_vector_start_index = WORLD_add_vertex(new_vector_start);

		new_sector_size += 1;
	}
	else
	{
		new_vector_start = closest_vector;
		new_vector_start_index = closest_vector_index;
	}

	first_vector = new_vector_start;
	first_vector_index = new_vector_start_index;
}

void new_edge()
{
	VECTOR2 new_vector_end;
	int new_vector_end_index;

	if(closest_vector_distance >= 0.1f)
	{
		if(snap_to_grid)
			new_vector_end = get_closest_grid(editor_cursor);
		else
			new_vector_end = editor_cursor;

		new_vector_end_index = WORLD_add_vertex(new_vector_end);

		WORLD_add_edge_to_sector(creating_sector, new_vector_start_index, new_vector_end_index);

		new_vector_start = new_vector_end;
		new_vector_start_index = new_vector_end_index;

		new_sector_size += 1;
	}
	else
	{
		new_vector_end = closest_vector;
		new_vector_end_index = closest_vector_index;

		WORLD_add_edge_to_sector(creating_sector, new_vector_start_index, new_vector_end_index);

		if(first_vector_index == closest_vector_index)
		{
			drawing_sector = 0;
			occupied = 0;
			new_sector_size = 0;
			save_sector();
			return;
		}

		new_vector_start = new_vector_end;
		new_vector_start_index = new_vector_end_index;
	}
}

void delete_vertex()
{

	WORLD_delete_vertex_at(closest_vector_index);
}
/*
void draw_cursor()
{
	POINT2 cursor_ss;

	cursor_ss = convert_ws_to_editor_ss(editor_cursor);

	GFX_set_pixel(engine.screen, cursor_ss.x, cursor_ss.y + 3, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x, cursor_ss.y + 2, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x, cursor_ss.y - 2, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x, cursor_ss.y - 3, GFX_Map_Color(cursor_color), 0);

	GFX_set_pixel(engine.screen, cursor_ss.x + 3, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x + 2, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x - 2, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
	GFX_set_pixel(engine.screen, cursor_ss.x - 3, cursor_ss.y, GFX_Map_Color(cursor_color), 0);
}
*
void draw_editor_map()
{
	int last_v;
	unsigned int color;

	VECTOR2 last_v_vector;
	VECTOR2 current_v_vector;

	POINT2 current_line_start;
	POINT2 current_line_end;

	for(int s = 0; s < loaded_level.s_num; s++)
	{
		for(int e = 0; e < (loaded_level.sectors+s)->e_num; e++)
		{
			EDGE current_edge = *((loaded_level.sectors+s)->e + e);
			
			last_v_vector = *(loaded_level.vertexes + current_edge.v_start);
			current_v_vector = *(loaded_level.vertexes + current_edge.v_end);

			current_line_start = convert_ws_to_editor_ss(last_v_vector);
			current_line_end = convert_ws_to_editor_ss(current_v_vector);

			if(current_edge.is_portal)
			{
				color = SDL_MapRGB(engine.screen->format, 60, 60, 60);
			}
			else
			{
				color = SDL_MapRGB(engine.screen->format, 180, 180, 180);
			}

			GFX_draw_line(engine.screen, current_line_start, current_line_end, color);
		}
	}
}

void draw_new_sector_preview()
{
	if(drawing_sector)
	{
		EDGE current_edge;

		for(int i = 0; i < creating_sector->e_num; i++)
		{
			current_edge = creating_sector->e[i];
			GFX_draw_line(	engine.screen, 
							convert_ws_to_editor_ss(get_vertex_from_sector(creating_sector, i, 0)),
							convert_ws_to_editor_ss(get_vertex_from_sector(creating_sector, i, 1)),
							GFX_Map_Color(GFX_Color(255, 0, 0)));
		}

		VECTOR2 preview_cursor;

		if(snap_to_grid)
			preview_cursor = get_closest_grid(editor_cursor);
		else
			preview_cursor = editor_cursor;

		GFX_draw_line(	engine.screen, 
						convert_ws_to_editor_ss(new_vector_start),
						convert_ws_to_editor_ss(preview_cursor),
						GFX_Map_Color(GFX_Color(255, 0, 0)));
	}
}


void draw_ui()
{
	int location_x = 180;

	if(show_info == 1)
	{

		GFX_draw_tiny_string(point2(location_x,  68), "Arrows : Move cursor.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x,  74), "WASD : Pan camera.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x,  80), "P : Go to play/3d mode.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x,  86), "M : Grab closest vertex.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x,  92), "G : Toggle snap to grid.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x,  98), "+ : Increase grid size.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x, 104), "- : Decrease grid size.", GFX_Tint(1.0, 0.3, 0.0));

		GFX_draw_tiny_string(point2(location_x, 116), "Drawing sector mode", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x, 122), "Space : Start drawing sector mode.", GFX_Tint(1.0, 0.3, 0.0));
		GFX_draw_tiny_string(point2(location_x, 128), "Space : Add vertex.", GFX_Tint(1.0, 0.3, 0.0));

		GFX_draw_tiny_string(point2(location_x, 140), "Del : Delete closest vertex.", GFX_Tint(1.0, 0.3, 0.0));
	}
	
	if(snap_to_grid == 1)
		GFX_draw_string(point2(0,0), "Snap to grid", GFX_Map_Color(GFX_Color(255, 0, 100)));
	if(drawing_sector == 1)
		GFX_draw_string(point2(0,8), "Drawing Sector", GFX_Map_Color(GFX_Color(255, 0, 100)));

	if(grabbed == 1)
	{
		sprintf(buffer, "x = %f", grabbed_vector_location.x);
		GFX_draw_string(point2(0, 240-18), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		sprintf(buffer, "y = %f", grabbed_vector_location.y);
		GFX_draw_string(point2(0, 240-9), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
	}

	if(show_info == 1)
	{
		sprintf(buffer, "SID = %i", closest_sector_index);
		GFX_draw_string(point2(120, 240-18), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		sprintf(buffer, "EID = %i", closest_edge_index);
		GFX_draw_string(point2(120, 240-9), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		sprintf(buffer, "PORTAL = %i", closest_edge->is_portal);
		GFX_draw_string(point2(188, 240-18), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));

		if(closest_edge->is_portal)
		{
			sprintf(buffer, "NID = %i", closest_edge->neighbor_sector_id);
			GFX_draw_string(point2(188, 240-9), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		}

		sprintf(buffer, "S=%i", loaded_level.s_num);
		GFX_draw_string(point2(274, 240-27), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		sprintf(buffer, "E=%i", (loaded_level.sectors + closest_sector_index)->e_num);
		GFX_draw_string(point2(274, 240-18), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		sprintf(buffer, "V=%i", loaded_level.v_num);
		GFX_draw_string(point2(274, 240-9), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		
		if(grabbed == 0)
		{
			sprintf(buffer, "x = %f", editor_cursor.x);
			GFX_draw_string(point2(0, 240-18), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
			sprintf(buffer, "y = %f", editor_cursor.y);
			GFX_draw_string(point2(0, 240-9), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
		}
	}

	if(show_texture_select)
	{
		EDITOR_draw_texture_select();
	}

	if(is_console_open())
	{
		GFX_draw_console();
	}
}

void EDITOR_open_texture_select(int ceil_floor_wall)
{
	selecting_texture_for = ceil_floor_wall;

	if(selecting_texture_for == 0)
	{
		selected_texture_param = get_sector_at(closest_sector_index)->text_param_ceil;
	}

	if(selecting_texture_for == 1)
	{
		selected_texture_param  = get_sector_at(closest_sector_index)->text_param_floor;
	}

	if(selecting_texture_for == 2)
	{
		selected_texture_param = get_edge_at(get_sector_at(closest_sector_index), closest_edge_index)->text_param;
	}

	selected_texture.y = selected_texture_param.id / 8;
	selected_texture.x = selected_texture_param.id % 8;

	show_texture_select = 1;
	occupied = 1;
}

void EDITOR_draw_texture_select()
{
	unsigned int pixel;

	if(selecting_texture_for == EDIT_TEXTURE_CEIL)
	{
		sprintf(buffer, "Selecting ceiling texture");
		GFX_draw_string(point2(0, 0), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
	}

	if(selecting_texture_for == EDIT_TEXTURE_FLOOR)
	{
		sprintf(buffer, "Selecting floor texture");
		GFX_draw_string(point2(0, 0), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
	}

	if(selecting_texture_for == EDIT_TEXTURE_EDGE)
	{
		sprintf(buffer, "Selecting wall texture");
		GFX_draw_string(point2(0, 0), buffer, GFX_Map_Color(GFX_Color(255, 100, 0)));
	}

	if(engine.game_mode == 0)
	{
		sprintf(buffer, "Offset U = %i V = %i", selected_texture_param.u_offset, selected_texture_param.v_offset);
		GFX_draw_string(point2(0, 8), buffer, GFX_Map_Color(GFX_Color(200, 70, 0)));
		sprintf(buffer, "Scale U = %f V = %f", selected_texture_param.u_scale, selected_texture_param.v_scale);
		GFX_draw_string(point2(0, 16), buffer, GFX_Map_Color(GFX_Color(200, 70, 0)));
		sprintf(buffer, "Parallax = %i", selected_texture_param.parallax);
		GFX_draw_string(point2(0, 24), buffer, GFX_Map_Color(GFX_Color(200, 70, 0)));
	}

	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 32; y++)
		{
			for(int t_x = 0; t_x < 32; t_x++)
			{
				for(int t_y = 0; t_y < 32; t_y++)
				{
					if(loaded_textures[x + y * 8].loaded)
					{
						int draw_texture = 0;

						if(selected_texture.x != x || selected_texture.y != y)
							draw_texture = 1;
						else
							draw_texture = engine_blink_state();

						if(draw_texture)
						{
							pixel = GFX_get_pixel(loaded_textures[x + y*8].surface, t_x * 4, t_y * 4);
							GFX_set_pixel(engine.screen, 32 + 32 * x + t_x, 32 + 32 * y + t_y, pixel, 0);
						}
					}
				}
			}
		}
	}
}

void EDITOR_move_texture_select(POINT2 amount)
{
	POINT2 to_location;

	to_location = sum_p2(selected_texture, amount);

	if(	to_location.x >= 0 && to_location.x < 8 &&
		to_location.y >= 0 && to_location.y < 32)
	{
		if(loaded_textures[to_location.x + to_location.y * 8].loaded)
		{
			selected_texture = to_location;
		}
	}
}

void EDITOR_apply_texture_select()
{
	selected_texture_param.id = selected_texture.x + selected_texture.y * 8;

	if(selecting_texture_for == 0)
	{
		get_sector_at(closest_sector_index)->text_param_ceil = selected_texture_param;
	}

	if(selecting_texture_for == 1)
	{
		get_sector_at(closest_sector_index)->text_param_floor = selected_texture_param;
	}

	if(selecting_texture_for == 2)
	{
		get_edge_at(get_sector_at(closest_sector_index), closest_edge_index)->text_param = selected_texture_param;
	}

	occupied = 0;
	show_texture_select = 0;
}

void EDITOR_cancel_texture_select()
{
	occupied = 0;
	show_texture_select = 0;
}

void change_closest_texture_params(int invert_parallax, POINT2 add_offset, VECTOR2 add_scale)
{
	GFX_TEXTURE_PARAM closest_texture_param;

	switch(selecting_texture_for)
	{
		case EDIT_TEXTURE_FLOOR:
			closest_texture_param = player->closest_sector->text_param_floor;
			break;
		case EDIT_TEXTURE_CEIL:
			closest_texture_param = player->closest_sector->text_param_ceil;
			break;
		case EDIT_TEXTURE_EDGE:
			closest_texture_param = player->closest_edge->text_param;
			break;
	}

	if(invert_parallax) closest_texture_param.parallax = !closest_texture_param.parallax;

	closest_texture_param.u_offset = closest_texture_param.u_offset + add_offset.x;
	closest_texture_param.v_offset = closest_texture_param.v_offset + add_offset.y;

	closest_texture_param.u_scale = closest_texture_param.u_scale + add_scale.x;
	closest_texture_param.v_scale = closest_texture_param.v_scale + add_scale.y;

	switch(selecting_texture_for)
	{
		case EDIT_TEXTURE_FLOOR:
			player->closest_sector->text_param_floor = closest_texture_param;
			break;
		case EDIT_TEXTURE_CEIL:
			player->closest_sector->text_param_ceil = closest_texture_param;
			break;
		case EDIT_TEXTURE_EDGE:
			player->closest_edge->text_param = closest_texture_param;
			break;
	}
}		

void draw_closest_markers()
{
	POINT2 closest_vector_ss;
	POINT2 closest_projection_ss;

	closest_vector_ss = convert_ws_to_editor_ss(closest_vector);
	closest_projection_ss = convert_ws_to_editor_ss(closest_edge_projection);

	unsigned int line_color;

	if(closest_edge->is_portal == 1)
	{
		line_color = GFX_Map_Color(GFX_Color(110, 0, 0));
	}
	else
	{
		line_color = GFX_Map_Color(GFX_Color(210, 0, 0));
	}

	GFX_draw_line(engine.screen, convert_ws_to_editor_ss(get_vertex_at(closest_edge->v_start)), convert_ws_to_editor_ss(get_vertex_at(closest_edge->v_end)), line_color);

	GFX_set_pixel(engine.screen, closest_projection_ss.x, closest_projection_ss.y, GFX_Map_Color(GFX_Color(0, 0, 255)), 0);

	GFX_set_pixel(engine.screen, closest_vector_ss.x, closest_vector_ss.y, GFX_Map_Color(GFX_Color(255, 0, 0)), 0);
}

void EDITOR_Render()
{
	if(SDL_MUSTLOCK(engine.screen))
	{
		if(SDL_LockSurface(engine.screen) < 0)
		{
			printf("Couldnt lock screen: %s\n", SDL_GetError());
			return;
		}
	}

	GFX_clear_screen();

	draw_grid();

	draw_editor_map();
	draw_new_sector_preview();
	draw_closest_markers();

	draw_cursor();

	draw_ui();

	if(show_fps)
	{
		sprintf(buffer, "%f", engine.current_fps);
		GFX_draw_string(point2(0, 0), buffer, SDL_MapRGB(engine.screen->format, 255, 255, 0));
	}

	if(SDL_MUSTLOCK(engine.screen))
	{
		SDL_UnlockSurface(engine.screen);
	}

	SDL_UpdateRect(engine.screen, 0, 0, engine.gfx->screen_res_x * engine.pixel_scale, engine.gfx->screen_res_y * engine.pixel_scale);
}

void grab_vertex()
{
	grabbed = 1;
	occupied = 1;
	
	grabbed_vector_index = closest_vector_index;
	grabbed_vector_start = closest_vector;
}

void drop_vertex()
{
	grabbed = 0;
	occupied = 0;

	if(snap_to_grid)
		loaded_level.vertexes[grabbed_vector_index] = grabbed_vector_location;
	else
		loaded_level.vertexes[grabbed_vector_index] = grabbed_vector_location;
}

void cancel_grab()
{
	grabbed = 0;
	occupied = 0;

	loaded_level.vertexes[grabbed_vector_index] = grabbed_vector_start;
}

void cancel_new_sector()
{
	drawing_sector = 0;
	occupied = 0;

	free(creating_sector->e);
	free(creating_sector);

	WORLD_remove_n_vertexes(new_sector_size);

	new_sector_size = 0;
}

extern bool e_running;
SDL_Event event;

void EDITOR_Handle_Input()
{
	if(is_console_open())
	{
		INPUT_Handle_Console();
	}
	else
	{
		unsigned char * keystate = SDL_GetKeyState(NULL); 

		if(keystate[SDLK_LSHIFT])
		{
			cursor_speed = run_cursor_speed;
		}
		else if(keystate[SDLK_LCTRL])
		{
			cursor_speed = crawl_cursor_speed;
		}
		else
		{
			cursor_speed = walk_cursor_speed;
		}

		if(keystate[SDLK_w] && show_texture_select == 0)
		{
			move_view(scale_v2(vector2(0, 1), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_s] && show_texture_select == 0)
		{
			move_view(scale_v2(vector2(0, -1), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_d] && show_texture_select == 0)
		{
			move_view(scale_v2(vector2(1, 0), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_a] && show_texture_select == 0)
		{
			move_view(scale_v2(vector2(-1, 0), cursor_speed * engine_delta_time()));
		}

		if(keystate[SDLK_UP] && show_texture_select == 0)
		{
			move_cursor(scale_v2(vector2(0, 1), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_DOWN] && show_texture_select == 0)
		{
			move_cursor(scale_v2(vector2(0, -1), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_RIGHT] && show_texture_select == 0)
		{
			move_cursor(scale_v2(vector2(1, 0), cursor_speed * engine_delta_time()));
		}

		if(keystate[SDLK_LEFT] && show_texture_select == 0)
		{
			move_cursor(scale_v2(vector2(-1, 0), cursor_speed * engine_delta_time()));
		}
		
		if(keystate[SDLK_PAGEUP] && show_texture_select == 0)
		{
			editor_zoom *= 1.0f + 2.0f * engine_delta_time();
		}
		
		if(keystate[SDLK_PAGEDOWN] && show_texture_select == 0)
		{
			editor_zoom *= 1.0f - 2.0f * engine_delta_time();
		}

		while(SDL_PollEvent(&event) != 0)
		{
			if(event.type == SDL_QUIT)
			{
				engine.running = false;
			}
			else if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == '`' || event.key.keysym.sym == '\'' || event.key.keysym.sym == 'q')
				{ 				
					set_console_open(!is_console_open());
				}

				if(event.key.keysym.sym == 'm')
				{
					if(grabbed == 0 && occupied == 0)
						grab_vertex();
					else if(grabbed == 1)
						drop_vertex();
				}

				if(event.key.keysym.sym == 'p' && show_texture_select == 0)
				{
					if(occupied == 0)
					{	
						engine.game_mode = !engine.game_mode;
					}
				}
				else if(event.key.keysym.sym == 'p' && show_texture_select)
				{
					selected_texture_param.parallax = !selected_texture_param.parallax; 
				}

				if(event.key.keysym.sym == 'i')
				{
					show_info = !show_info;
				}

				if(event.key.keysym.sym == 'g')
					snap_to_grid = !snap_to_grid;

				if(event.key.keysym.sym == 't' && occupied == 0)
				{
					EDITOR_open_texture_select(2);
				}
				else if(event.key.keysym.sym == 't' && show_texture_select)
					EDITOR_cancel_texture_select();

				if(event.key.keysym.sym == 'y' && occupied == 0)
				{
					EDITOR_open_texture_select(0);
				}
				else if(event.key.keysym.sym == 'y' && show_texture_select)
					EDITOR_cancel_texture_select();

				if(event.key.keysym.sym == 'r' && occupied == 0)
				{
					EDITOR_open_texture_select(1);
				}
				else if(event.key.keysym.sym == 'r' && show_texture_select)
					EDITOR_cancel_texture_select();

				if(event.key.keysym.sym == '+' || event.key.keysym.sym == '=')
				{
					grid_size += 0.1f;
				}

				if(event.key.keysym.sym == '_' || event.key.keysym.sym == '-')
				{	
					if(grid_size >= 0.15f) grid_size -= 0.1f;
				}
				
				if(event.key.keysym.sym == SDLK_RETURN)
				{
					if(grabbed)
						drop_vertex();

					if(show_texture_select)
						EDITOR_apply_texture_select();
				}

				if(event.key.keysym.sym == SDLK_ESCAPE)
				{
					if(grabbed)
						cancel_grab();

					if(drawing_sector)
						cancel_new_sector();

					if(show_texture_select)
						EDITOR_cancel_texture_select();
				}

				if(event.key.keysym.sym == SDLK_DELETE && occupied == 0)
				{
					delete_vertex();
				}

				if(event.key.keysym.sym == SDLK_SPACE)
				{
					if(drawing_sector == 0 && occupied == 0)
						new_sector();
					else if(drawing_sector == 1)
						new_edge();
				}

				if(show_texture_select)
				{
					if(event.key.keysym.sym == SDLK_UP)
						EDITOR_move_texture_select(point2(0, -1));

					if(event.key.keysym.sym == SDLK_DOWN)
						EDITOR_move_texture_select(point2(0, 1));

					if(event.key.keysym.sym == SDLK_RIGHT)
						EDITOR_move_texture_select(point2(1, 0));

					if(event.key.keysym.sym == SDLK_LEFT)
						EDITOR_move_texture_select(point2(-1, 0));

					if(keystate[SDLK_LSHIFT])
					{
						if(event.key.keysym.sym == SDLK_u)
							selected_texture_param.v_scale -= 0.1; 

						if(event.key.keysym.sym == SDLK_j)
							selected_texture_param.v_scale += 0.1; 

						if(event.key.keysym.sym == SDLK_k)
							selected_texture_param.u_scale += 0.1; 

						if(event.key.keysym.sym == SDLK_h)
							selected_texture_param.u_scale -= 0.1; 
					}
					else
					{
						if(event.key.keysym.sym == SDLK_u)
							selected_texture_param.v_offset -= 1; 

						if(event.key.keysym.sym == SDLK_j)
							selected_texture_param.v_offset += 1; 

						if(event.key.keysym.sym == SDLK_k)
							selected_texture_param.u_offset += 1; 

						if(event.key.keysym.sym == SDLK_h)
							selected_texture_param.u_offset -= 1; 
					}
				}
			}
		}
	}
}

void EDIT_MODE_Handle_Input()
{
	unsigned char * keystate = SDL_GetKeyState(NULL); 
		
	if(keystate[SDLK_LSHIFT])
	{
		player->speed = player->run_speed;
		player->turn_speed = player->run_turn_speed;
	}
	else
	{
		player->speed = player->walk_speed;
		player->turn_speed = player->walk_turn_speed;
	}

	if(keystate[SDLK_UP])
	{
		PLAYER_Move(player, scale_v2(rot_v2(vector2(0, 1), -(player->facing)), player->speed * engine_delta_time()));
	}
	
	if(keystate[SDLK_DOWN])
	{
		PLAYER_Move(player, scale_v2(rot_v2(vector2(0, -1), -(player->facing)), player->speed * engine_delta_time()));
	}
	
	if(keystate[SDLK_RIGHT])
	{
		if(keystate[SDLK_LALT])
		{
			PLAYER_Move(player, scale_v2(rot_v2(vector2(1, 0), -(player->facing)), player->speed * engine_delta_time()));
		}
		else
		{
			PLAYER_Turn(player, player->turn_speed * engine_delta_time());
		}
	}
	
	if(keystate[SDLK_LEFT])
	{
		if(keystate[SDLK_LALT])
		{
			PLAYER_Move(player, scale_v2(rot_v2(vector2(-1, 0), -(player->facing)), player->speed * engine_delta_time()));
		}
		else
		{
			PLAYER_Turn(player, -player->turn_speed * engine_delta_time());
		}
	}

	if(keystate[SDLK_PAGEUP])
	{
		player->pos_height += player->speed * engine_delta_time();
	}
	
	if(keystate[SDLK_PAGEDOWN])
	{
		player->pos_height -= player->speed * engine_delta_time();
	}

	while(SDL_PollEvent(&event) != 0)
	{
		if(event.type == SDL_QUIT)
		{
			engine.running = false;
		}
		else if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case '`':
					set_console_open(!is_console_open());
					break;

				case 'i':
					show_fps = !show_fps;
					break;

				case 'p':
					if(engine.edit_mode == 1 && show_texture_select == 0) engine.game_mode = !engine.game_mode;
					break;

				case 'f':
					if(engine.edit_mode == 1 && show_texture_select == 0)
						change_closest_texture_params(1, ZERO_POINT2, ZERO_VECTOR2);
					break;

				case 'u':
					if(engine.edit_mode == 1 && show_texture_select == 0)
						if(!keystate[SDLK_LSHIFT])
							change_closest_texture_params(0, point2(0, -1), ZERO_VECTOR2);
						else
							change_closest_texture_params(0, ZERO_POINT2, vector2(0., -0.1));
					break;

				case 'j':
					if(engine.edit_mode == 1 && show_texture_select == 0)
						if(!keystate[SDLK_LSHIFT])
							change_closest_texture_params(0, point2(0, 1), ZERO_VECTOR2);
						else
							change_closest_texture_params(0, ZERO_POINT2, vector2(0., 0.1));
					break;

				case 'h':
					if(engine.edit_mode == 1 && show_texture_select == 0)
						if(!keystate[SDLK_LSHIFT])
							change_closest_texture_params(0, point2(-1, 0), ZERO_VECTOR2);
						else
							change_closest_texture_params(0, ZERO_POINT2, vector2(-0.1, 0.));
					break;

				case 'k':
					if(engine.edit_mode == 1 && show_texture_select == 0)
						if(!keystate[SDLK_LSHIFT])
							change_closest_texture_params(0, point2(1, 0), ZERO_VECTOR2);
						else
							change_closest_texture_params(0, ZERO_POINT2, vector2(0.1, 0.));
					break;

				case 'r':
					if(engine.edit_mode == 1 && show_texture_select == 0) selecting_texture_for = 0;
					break;

				case 't':
					if(engine.edit_mode == 1 && show_texture_select == 0) selecting_texture_for = 2;
					break;

				case 'y':
					if(engine.edit_mode == 1 && show_texture_select == 0) selecting_texture_for = 1;
					break;

				case 'g':
					if(engine.edit_mode == 1 && show_texture_select == 0){EDITOR_open_texture_select(selecting_texture_for); player->movement_blocked = 1;}
					break;

				case SDLK_F1:
					if(engine.edit_mode) show_help = !show_help;
					break;

				case SDLK_UP:
					if(engine.edit_mode) EDITOR_move_texture_select(point2(0, -1));
					break;

				case SDLK_DOWN:
					if(engine.edit_mode) EDITOR_move_texture_select(point2(0, 1));
					break;

				case SDLK_RIGHT:
					if(engine.edit_mode) EDITOR_move_texture_select(point2(1, 0));
					break;

				case SDLK_LEFT:
					if(engine.edit_mode) EDITOR_move_texture_select(point2(-1, 0));
					break;

				case SDLK_RETURN:
					if(engine.edit_mode && show_texture_select) {EDITOR_apply_texture_select(); player->movement_blocked = 0;}
					break;

				case SDLK_HOME:
					if(engine.edit_mode)
					{
						if(selecting_texture_for == EDIT_TEXTURE_CEIL) player->closest_sector->ceiling_height += 0.1;
						if(selecting_texture_for == EDIT_TEXTURE_FLOOR) player->closest_sector->floor_height += 0.1;
					}
					break;

				case SDLK_END:
					if(engine.edit_mode)
					{
						if(selecting_texture_for == EDIT_TEXTURE_CEIL) player->closest_sector->ceiling_height -= 0.1;
						if(selecting_texture_for == EDIT_TEXTURE_FLOOR) player->closest_sector->floor_height -= 0.1;
					}
					break;
			}
		}
	}
}

void EDITOR_Loop()
{
	update_borders();

	clip_cursor();

	if(grabbed == 1 && snap_to_grid == 0)
	{
		grabbed_vector_location = editor_cursor;
		loaded_level.vertexes[grabbed_vector_index] = editor_cursor;
	}
	else if(grabbed == 1 && snap_to_grid == 1)
	{
		grabbed_vector_location = get_closest_grid(editor_cursor);
		loaded_level.vertexes[grabbed_vector_index] = get_closest_grid(editor_cursor);
	}

	get_closest_vertex(editor_cursor, &closest_vector, &closest_vector_index, &closest_vector_distance);
	get_closest_edge(editor_cursor, &closest_edge, &closest_edge_projection, &closest_edge_index, &closest_sector_index, &closest_edge_distance);
	closest_sector = get_sector_at(closest_sector_index);
}

void EDITOR_Init()
{
	editor_center = vector2(0., 0.);
	editor_cursor = vector2(0., 0.);

	closest_edge = get_edge_from_level(0, 0);
}
*/