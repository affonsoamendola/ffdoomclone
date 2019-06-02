#ifndef _3D_H
#define _3D_H

#define GLOBAL_TEXTURE_SCALE 1.0f

void G3D_render_3d();

float get_view_plane_pos_x(int ssx);

float get_view_angle_from_ss(int ssx);

void G3D_draw_level();

void G3D_draw_wall(	int screen_x, 
					int top_visible, int top_invisible,
					int bot_visible, int bot_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters,
					TINT tint, 
					float global_texture_scale_height, float global_texture_scale_length);

void G3D_draw_sprite(VECTOR2 sprite_position, VECTOR2 sprite_size, float height);

void G3D_draw_sprite_wall (	VECTOR2 start_pos, VECTOR2 end_pos,
							float bot_height, float top_height,
							GFX_TEXTURE_PARAM texture_parameters,
							TINT tint);

void G3D_draw_sprite_wall_db (	VECTOR2 start_pos, VECTOR2 end_pos,
								float bot_height, float top_height,
								GFX_TEXTURE_PARAM texture_parameters,
								TINT tint);

void G3D_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters,
						TINT tint);

#endif 