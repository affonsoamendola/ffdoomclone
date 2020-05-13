#ifndef EDITOR_H
#define EDITOR_H

#include "ff_vector2.h"
#include "engine.h"
#include "gfx.h"

#include "effect.h"

typedef struct Editor_
{
	Vector2f cursor_location;
	Vector2f center;

	Color cursor_color;
	Color current_cursor_color;
	Effect cursor_effect;

	Vector2f viewport_top_left;
	Vector2f viewport_bottom_right;
	float viewport_size_x;
	float viewport_size_y;

	Color grid_color;
	float grid_size;

	float zoom;
	float max_zoom;
	float min_zoom;
	float zoom_speed;

	Color edge_color;
} Editor;

extern Editor editor;

static inline Point2 editor_ws_to_ss(const Vector2f pos, const Vector2f origin, const float zoom)
{
	return point2(	(int)((pos.x - origin.x)*zoom + gfx.screen_res_x/2),
					(int)(gfx.screen_res_y/2 - (pos.y - origin.y)*zoom));
}


static inline Vector2f ss_to_editor_ws(const Point2 pos, const Vector2f origin, const float zoom)
{
	return vector2f((float)(pos.x - gfx.screen_res_x/2)/zoom + origin.x,
					-((float)(pos.y - gfx.screen_res_y/2)/zoom - origin.y));
}

static inline void get_editor_bounds(Vector2f* top_left, Vector2f* bot_right)
{
	*top_left = ss_to_editor_ws(point2(0,0), editor.center, editor.zoom);
	*bot_right = ss_to_editor_ws(point2(gfx.screen_res_x, gfx.screen_res_y), editor.center, editor.zoom);
}

void init_editor();

void enter_editor();
void leave_editor();

void update_editor();

void scroll_wheel_zoom(void* zoom, int direction);

void draw_editor();
void draw_editor_cursor(const Vector2f location, const Color color);

void move_editor_cursor(void* editor_cursor, Vector2f delta_mouse);
void move_editor_view(void* editor_center, Vector2f delta_mouse);

void draw_grid();
void draw_map();

static inline Vector2f get_closest_grid(const Vector2f pos, const float grid_size)
{
	return vector2f(round(pos.x/grid_size) * grid_size, round(pos.y/grid_size) * grid_size);
}
/*
#include "vector2.h"

void move_cursor(VECTOR2 amount);
void move_view(VECTOR2 amount);

POINT2 convert_ws_to_editor_ss(VECTOR2 pos);
VECTOR2 convert_editor_ss_to_ws(POINT2 pos);

void EDITOR_Render();
void EDITOR_Handle_Input();
void EDITOR_Loop();

void EDITOR_Init();

void EDITOR_open_texture_select(int ceil_floor_wall);
void EDITOR_draw_texture_select();
void EDITOR_move_texture_select(POINT2 amount);
void EDITOR_apply_texture_select();
void EDITOR_cancel_texture_select();

void EDIT_MODE_Handle_Input();
*/
#endif
