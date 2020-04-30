
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "engine.h"

#include "vector2.h"
#include "point2.h"
#include "gfx.h"
#include "world.h"
#include "list.h"
#include "player.h"
#include "utility.h"

#include "3d.h"

extern PLAYER * player;
extern LEVEL loaded_level;
extern CAMERA * main_camera;
extern SDL_Surface * screen;

void  G3D_transform_no_height(	VECTOR2 origin, float origin_rotation,
								VECTOR2 position, VECTOR2* out_pos)
{
	VECTOR2 transformed_pos;

	transformed_pos = sub_v2(position, origin);
	transformed_pos = rot_v2(transformed_pos, origin_rotation);

	*out_pos = transformed_pos;
}

void  G3D_transform(	VECTOR2 origin, float origin_rotation, float origin_height,
						VECTOR2 position, float height,
						VECTOR2* out_pos, float* out_height)
{
	G3D_transform_no_height(origin, origin_rotation, position, out_pos);
	
	*out_height = height - origin_height;
}

void G3D_project_no_height( VECTOR2 local_position, CAMERA* camera,
							POINT2* out_screen_coordinates, 
							float* out_x_scale, float* out_y_scale)
{
	float x_scale;
	float y_scale;

	float proj_x;

	POINT2 screen_coordinates;

	x_scale = camera->camera_parameters_x / local_position.y;
	y_scale = camera->camera_parameters_y / local_position.y;

	proj_x = local_position.x * x_scale;

	screen_coordinates.x = (int)(proj_x) + SCREEN_RES_X/2;
	screen_coordinates.y = SCREEN_RES_Y/2;

	*out_x_scale = x_scale;
	*out_y_scale = y_scale;
	*out_screen_coordinates = screen_coordinates;
}

void G3D_project( 	VECTOR2 local_position, float height, CAMERA* camera,
					POINT2* out_screen_coordinates, float* out_x_scale, float* out_y_scale)
{
	POINT2 screen_coordinates;
	float x_scale;
	float y_scale;

	G3D_project_no_height( 	local_position, camera,
							&screen_coordinates, 
							&x_scale, &y_scale);

	float proj_height;

	proj_height = height * y_scale;

	screen_coordinates.y = SCREEN_RES_Y/2 - (int)(proj_height);

	*out_x_scale = x_scale;
	*out_y_scale = y_scale;
	*out_screen_coordinates = screen_coordinates;
}

void G3D_render_3d()
{
	G3D_draw_level();
	G3D_draw_sprite(vector2(0., 2.), vector2(0.35f, 0.7f), 0);
}

float get_view_plane_pos_x(int ssx)
{
	float pos_x = ((float)(ssx - SCREEN_RES_X/2)/(float)(SCREEN_RES_X/2)) *  main_camera->hither_x;

	return pos_x;
}

float get_view_angle_from_ss(int ssx)
{
	float angle_x = atan(get_view_plane_pos_x(ssx) / main_camera->hither_z);

	return angle_x;
}

void G3D_draw_level()
{
	SECTOR * current_sector;
	EDGE * current_edge;
	VECTOR2 transformed_pos_0;
	VECTOR2 transformed_pos_1;

	LIST * pending_portals;

	int current_sector_id;

	int start_screen_x;
	int end_screen_x;

	int y_undrawn_top[SCREEN_RES_X], y_undrawn_bot[SCREEN_RES_X];

	clear_z_buffer(main_camera);

	for(int x = 0; x < SCREEN_RES_X; x++)
	{
		y_undrawn_top[x] = 0;
		y_undrawn_bot[x] = SCREEN_RES_Y; 
	} 

	start_screen_x = 0;
	end_screen_x = SCREEN_RES_X - 1;

	current_sector_id = player->current_sector;

	pending_portals = create_list();

	//Get current sector
	current_sector = loaded_level.sectors + current_sector_id;

	struct item { SECTOR * sector; int start_screen_x, end_screen_x;};

	struct item first_render = {current_sector, start_screen_x, end_screen_x};

	do
	{
		if(len_list(pending_portals) > 0)
		{
			struct item * current_render = (struct item *)pop_list(pending_portals);

			current_sector = current_render->sector;

			start_screen_x = current_render->start_screen_x;
			end_screen_x = current_render->end_screen_x;

			free (current_render);
		}

		for(int e = 0; e < current_sector->e_num; e++)
		{
			current_edge = current_sector->e + e;

			TINT current_tint = current_sector->tint;

			VECTOR2 edge_v0 = get_vertex_at(current_edge->v_start);
			VECTOR2 edge_v1 = get_vertex_at(current_edge->v_end);

			//Transform current edges to player point of view (player at 0,0)
			G3D_transform_no_height(	player->pos, player->facing,
										edge_v0, &transformed_pos_0);
			G3D_transform_no_height(	player->pos, player->facing,
										edge_v1, &transformed_pos_1);	

			VECTOR2 ni_pos_0 = transformed_pos_0;
			VECTOR2 ni_pos_1 = transformed_pos_1;

			float edge_length = norm_v2(sub_v2(ni_pos_1, ni_pos_0));

			float global_texture_scale_length = GLOBAL_TEXTURE_SCALE / edge_length;

			float t_u0 = 0.;
			float t_u1 = (float)(TEXTURE_SIZE_X);

			//If completely behind player, continue from loop
			if(transformed_pos_0.y <= 0 && transformed_pos_1.y <= 0) continue;
	
			//If partially behind player, intersect with player view area
			if(transformed_pos_0.y <= 0 || transformed_pos_1.y)
			{
				if(transformed_pos_0.y <= 0)
				{	
					VECTOR2 i0 = intersect_v2(	vector2(-(main_camera->hither_x), main_camera->hither_z), 
												vector2(-(main_camera->yon_x), main_camera->yon_z), 
												transformed_pos_0, transformed_pos_1);

					VECTOR2 i1 = intersect_v2(	vector2(main_camera->hither_x, main_camera->hither_z), 
												vector2(main_camera->yon_x, main_camera->yon_z), 
												transformed_pos_0, transformed_pos_1);

					if(i0.y > 0 && i1.y > 0)
					{
						if(i0.y < i1.y)
							transformed_pos_0 = i0;			
						else
							transformed_pos_0 = i1;			
					}
					else if(i0.y > 0)
						transformed_pos_0 = i0;
					else
						transformed_pos_0 = i1;
				}

				if(transformed_pos_1.y <= 0)
				//if(transformed_pos_1.x <= -(hither_x/hither_z)*transformed_pos_1.z)
				{	
					VECTOR2 i0 = intersect_v2(	vector2(-(main_camera->hither_x), main_camera->hither_z), 
												vector2(-(main_camera->yon_x), main_camera->yon_z), 
												transformed_pos_0, transformed_pos_1);

					VECTOR2 i1 = intersect_v2(	vector2(main_camera->hither_x, main_camera->hither_z), 
												vector2(main_camera->yon_x, main_camera->yon_z), 
												transformed_pos_0, transformed_pos_1);
						
					if(i0.y > 0 && i1.y > 0)
					{
						if(i0.y < i1.y)
							transformed_pos_1 = i0;
						else
							transformed_pos_1 = i1;
					}
					else if(i0.y > 0)
						transformed_pos_1 = i0;
					else
						transformed_pos_1 = i1;	
				}

				t_u0 = 	(transformed_pos_0.x - ni_pos_0.x) * 
						(TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);

				t_u1 = (transformed_pos_1.x - ni_pos_0.x) * 
						(TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);			
			}

			POINT2 screen_floor_0;
			POINT2 screen_floor_1;

			POINT2 screen_ceiling_0;
			POINT2 screen_ceiling_1;

			//Get relative ceil and floor heights

			float y_ceil = current_sector->ceiling_height - player->pos_height;
			float y_floor = current_sector->floor_height - player->pos_height;

			float sector_height = current_sector->ceiling_height - current_sector->floor_height;

			float global_texture_scale_height = GLOBAL_TEXTURE_SCALE / sector_height;

			float x_scale_0;
			float y_scale_0;

			float x_scale_1;
			float y_scale_1;

			G3D_project(transformed_pos_0, y_floor, main_camera,
						&screen_floor_0, &x_scale_0, &y_scale_0);

			G3D_project(transformed_pos_1, y_floor, main_camera,
						&screen_floor_1, &x_scale_1, &y_scale_1);
			
			//If outside screen, get out
			if(	screen_floor_0.x >= screen_floor_1.x || 
				screen_floor_1.x < start_screen_x || 
				screen_floor_0.x > end_screen_x) continue;

			//Project and get pixel position, like above.
			screen_ceiling_0 = screen_floor_0;
			screen_ceiling_1 = screen_floor_1;

			float proj_y0_ceiling = y_ceil * y_scale_0;
			float proj_y1_ceiling = y_ceil * y_scale_1;

			screen_ceiling_0.y = SCREEN_RES_Y/2 - (int)(proj_y0_ceiling);
			screen_ceiling_1.y = SCREEN_RES_Y/2 - (int)(proj_y1_ceiling);

			SECTOR * neighbor_sector;

			float nyceil= 0;
			float nyfloor= 0;

			int ny0ceiling;
			int ny0floor;

			int ny1ceiling;
			int ny1floor;

			float ny_global_texture_scale_height_top;
			float ny_global_texture_scale_height_bot;

			if(current_edge->is_portal)
			{
				neighbor_sector = loaded_level.sectors + current_edge->neighbor_sector_id;

				nyceil = neighbor_sector->ceiling_height - player->pos_height;
				nyfloor = neighbor_sector->floor_height - player->pos_height;

				ny_global_texture_scale_height_top = (-GLOBAL_TEXTURE_SCALE) / (neighbor_sector->ceiling_height - current_sector->ceiling_height);
				ny_global_texture_scale_height_bot = (GLOBAL_TEXTURE_SCALE) / (neighbor_sector->floor_height - current_sector->floor_height);
			}

			//Do the same for neighboring sectors
			ny0ceiling = SCREEN_RES_Y/2 - (int)(nyceil * y_scale_0);
			ny0floor = SCREEN_RES_Y/2 - (int)(nyfloor * y_scale_0);

			ny1ceiling = SCREEN_RES_Y/2 - (int)(nyceil * y_scale_1);
			ny1floor = SCREEN_RES_Y/2 - (int)(nyfloor * y_scale_1);

			int x_begin;
			int x_end;

			x_begin = max_int(screen_floor_0.x, start_screen_x);
			x_end = min_int(screen_floor_1.x, end_screen_x);

			for(int x = x_begin; x < x_end; x++)
			{
				float relative_x = (float)(x-screen_floor_0.x)/
									(float)(screen_floor_1.x-screen_floor_0.x);

				int screen_y_ceil = relative_x * (screen_ceiling_1.y - screen_ceiling_0.y) + screen_ceiling_0.y;
				int screen_y_floor = relative_x * (screen_floor_1.y - screen_floor_0.y) + screen_floor_0.y;

				int c_screen_y_ceil = clamp_int(screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
				int c_screen_y_floor = clamp_int(screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

				int text_x = (int)((t_u0*((screen_floor_1.x-x)*transformed_pos_1.y) + t_u1*((x-screen_floor_0.x)*transformed_pos_0.y)) / 
									((screen_floor_1.x-x)*transformed_pos_1.y + (x-screen_floor_0.x)*transformed_pos_0.y));
	
				VECTOR2 world_space;
				
				G3D_draw_visplane(	x, y_undrawn_top[x], c_screen_y_ceil,
									1, y_ceil - 0.02, 
									current_sector->text_param_ceil, 
									current_tint);

				G3D_draw_visplane(	x, c_screen_y_floor, y_undrawn_bot[x],
									0, y_floor + 0.02, 
									current_sector->text_param_floor, 
									current_tint);

				if(current_edge->is_portal)
				{
					int n_screen_y_ceil = relative_x * (ny1ceiling - ny0ceiling) + ny0ceiling;
					int n_screen_y_floor = relative_x * (ny1floor - ny0floor) + ny0floor;

					int c_n_screen_y_ceil = clamp_int(n_screen_y_ceil, y_undrawn_bot[x], y_undrawn_top[x]);
					int c_n_screen_y_floor = clamp_int(n_screen_y_floor, y_undrawn_bot[x], y_undrawn_top[x]);			

					//If wall between 2 sectors ceiling is visible

					if(c_n_screen_y_ceil > c_screen_y_ceil)
					{
						//Draw wall
						G3D_draw_wall(	x, 
										c_screen_y_ceil, screen_y_ceil, 
										c_n_screen_y_ceil, n_screen_y_ceil, 
										screen_floor_0.x, screen_floor_1.x+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
										current_edge->text_param, current_tint, 
										ny_global_texture_scale_height_top, global_texture_scale_length); 
					}

					y_undrawn_top[x] = clamp_int(max_int(c_screen_y_ceil, c_n_screen_y_ceil), SCREEN_RES_Y-1, y_undrawn_top[x]);

					//If wall between 2 sectors floor is visible
					if(c_n_screen_y_floor < c_screen_y_floor)
					{
						//Draw wall
						G3D_draw_wall(	x, 
										c_n_screen_y_floor, n_screen_y_floor, 
										c_screen_y_floor, screen_y_floor, 
										screen_floor_0.x, screen_floor_1.x+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
										current_edge->text_param, current_tint,
										ny_global_texture_scale_height_bot, global_texture_scale_length); 
					}

					y_undrawn_bot[x] = clamp_int(min_int(c_screen_y_floor, c_n_screen_y_floor), y_undrawn_bot[x], 0);
				}
				else
				{
					//Draw a normal wall
					G3D_draw_wall(	x, 
									c_screen_y_ceil, screen_y_ceil, 
									c_screen_y_floor, screen_y_floor, 
									screen_floor_0.x, screen_floor_1.x+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
									current_edge->text_param, current_tint,
									global_texture_scale_height, global_texture_scale_length);
				}
			}

			if(current_edge->is_portal)
			{
				struct item * pending_portal;
				pending_portal = malloc(sizeof(struct item));

				pending_portal->sector = loaded_level.sectors + current_edge->neighbor_sector_id;
				
				pending_portal->start_screen_x = x_begin;
				pending_portal->end_screen_x = x_end;

				append_list(pending_portals, pending_portal);
			}
		}
	}
	while(len_list(pending_portals) > 0);
}

void G3D_draw_wall(	int screen_x, 
					int top_visible, int top_invisible,
					int bot_visible, int bot_invisible,
					int x0, int x1, int u0, int u1, float z0, float z1,
					GFX_TEXTURE_PARAM texture_parameters,
					TINT tint, 
					float global_texture_scale_height, float global_texture_scale_length)
{
	int text_x = (int)((1./global_texture_scale_length)*((float)u0*((float)(x1-screen_x)*z1) + (float)u1*((float)(screen_x-x0)*z0)) / ((float)(x1-screen_x)*z1 + (float)(screen_x-x0)*z0));

	double z = (z1 - z0) * ((((screen_x-x0)*z0)) / ((x1-screen_x)*z1 + (screen_x-x0)*z0)) + z0;

	for(int screen_y = top_visible; screen_y < bot_visible; screen_y++)
	{
		if(texture_parameters.parallax)
		{
			int offset = (player->facing)/(2.*PI) * SKYBOX_SIZE_X;
			GFX_set_pixel_from_texture(	screen,
										texture_parameters,
										screen_x, screen_y,
										screen_x + offset, screen_y);
		}
		else
		{
			int text_y = (int)((float)(1./global_texture_scale_height)*(screen_y - top_invisible)/(float)(bot_invisible - top_invisible) * (TEXTURE_SIZE_Y));

			GFX_set_pixel_from_texture_depth_tint(	screen,
													texture_parameters,
													screen_x, screen_y,
													text_x, text_y, 
													z, tint);

			set_z_buffer(main_camera, screen_x, screen_y, z);
		}
	}	
}


//Draws a sprite on screen, taking into account the depth buffer.
void G3D_draw_sprite(	VECTOR2 sprite_position, 
						VECTOR2 sprite_size, 
						float height)
{
	
	GFX_TEXTURE_PARAM texture;

	texture.id = 11;
	texture.parallax = 0;

	texture.u_offset = 0;
	texture.v_offset = 0;

	texture.u_scale = 1.;
	texture.v_scale = 1.;

	VECTOR2 transformed_pos;
	float transformed_height;

	//Transforms the sprite coordinates to local coordinates,
	//in relation to the player, the Y parameter becomes distance from player.
	G3D_transform(	player->pos, player->facing, player->pos_height,
					sprite_position, height,
					&transformed_pos, &transformed_height);

	POINT2 screen_coords;
	float x_scale;
	float y_scale;

	//Projects sprite position to screen coordinates.
	G3D_project( 	transformed_pos, transformed_height, main_camera,
					&screen_coords, &x_scale, &y_scale);

	//Scales the sprite in accordance to distance.
	int ss_sprite_size_x = (int)(sprite_size.x * x_scale);
	int ss_sprite_size_y = (int)(sprite_size.y * y_scale);

	POINT2 screen_0 = point2(	screen_coords.x - ss_sprite_size_x/2,
								screen_coords.y - ss_sprite_size_y);

	POINT2 screen_1 = point2(	screen_coords.x + ss_sprite_size_x/2,
								screen_coords.y);

	//Clamps the sprite image.
	POINT2 clamped_screen_0 = clamp_p2(screen_0, point2(0, 0), point2(SCREEN_RES_X, SCREEN_RES_Y));
	POINT2 clamped_screen_1 = clamp_p2(screen_1, point2(0, 0), point2(SCREEN_RES_X, SCREEN_RES_Y));


	//Applies a tint just to test if tinting it works.
	TINT tint;

	tint.r = 0.1;
	tint.g = 0.1;
	tint.b = 0.6;

	for(int x = clamped_screen_0.x; x < clamped_screen_1.x; x++)
	{
		for(int y = clamped_screen_0.y; y < clamped_screen_1.y; y++)
		{
			if(transformed_pos.y < get_z_buffer(main_camera, x, y))
			{
				GFX_set_pixel_from_texture_tint(	screen, texture, x, y,
													(int)((float)(x-screen_0.x)/(float)(screen_1.x-screen_0.x) * 128),
													(int)((float)(y-screen_0.y)/(float)(screen_1.y-screen_0.y) * 256),
													tint);
			}
		}
	}
}

void G3D_draw_sprite_wall (	VECTOR2 start_pos, VECTOR2 end_pos,
							float bot_height, float top_height,
							GFX_TEXTURE_PARAM texture_parameters,
							TINT tint)
{
	//Transform current edges to player point of view (player at 0,0)
	VECTOR2 transformed_pos_0 = sub_v2(start_pos, player->pos);
	VECTOR2 transformed_pos_1 = sub_v2(end_pos, player->pos);
	
	transformed_pos_0 = rot_v2(transformed_pos_0, player->facing);
	transformed_pos_1 = rot_v2(transformed_pos_1, player->facing);

	VECTOR2 ni_pos_0 = transformed_pos_0;
	VECTOR2 ni_pos_1 = transformed_pos_1;

	float t_u0 = 0.;
	float t_u1 = (float)(TEXTURE_SIZE_X);

	//If completely behind player, continue from loop
	if(transformed_pos_0.y <= 0 && transformed_pos_1.y <= 0) return;

	//If partially behind player, intersect with player view area
	if(transformed_pos_0.y <= 0 || transformed_pos_1.y)
	{
		if(transformed_pos_0.y <= 0)
		{	
			VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);

			if(i0.y > 0 && i1.y > 0)
			{
				if(i0.y < i1.y)
					transformed_pos_0 = i0;			
				else
					transformed_pos_0 = i1;			
			}
			else if(i0.y > 0)
				transformed_pos_0 = i0;
			else
				transformed_pos_0 = i1;
		}

		if(transformed_pos_1.y <= 0)
		//if(transformed_pos_1.x <= -(hither_x/hither_z)*transformed_pos_1.z)
		{	
			VECTOR2 i0 = intersect_v2(vector2(-(main_camera->hither_x), main_camera->hither_z), vector2(-(main_camera->yon_x), main_camera->yon_z), transformed_pos_0, transformed_pos_1);
			VECTOR2 i1 = intersect_v2(vector2(main_camera->hither_x, main_camera->hither_z), vector2(main_camera->yon_x, main_camera->yon_z), transformed_pos_0, transformed_pos_1);
				
			if(i0.y > 0 && i1.y > 0)
			{
				if(i0.y < i1.y)
					transformed_pos_1 = i0;
				else
					transformed_pos_1 = i1;
			}
			else if(i0.y > 0)
				transformed_pos_1 = i0;
			else
				transformed_pos_1 = i1;	
		}

		t_u0 = (transformed_pos_0.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);
		t_u1 = (transformed_pos_1.x - ni_pos_0.x) * (TEXTURE_SIZE_X)/(ni_pos_1.x - ni_pos_0.x);			
	}

	//Do projection scales
	float xscale0 = main_camera->camera_parameters_x / transformed_pos_0.y;
	float yscale0 = main_camera->camera_parameters_y / transformed_pos_0.y;

	float xscale1 = main_camera->camera_parameters_x / transformed_pos_1.y;
	float yscale1 = main_camera->camera_parameters_y / transformed_pos_1.y;

	//Transform to pixel locations (and project)

	float proj_x0 = transformed_pos_0.x * xscale0;
	float proj_x1 = transformed_pos_1.x * xscale1;

	int x0 = (int)(proj_x0) + SCREEN_RES_X/2;
	int x1 = (int)(proj_x1) + SCREEN_RES_X/2;

	//If outside screen, get out
	if(x0 >= x1 || x1 < 0 || x0 > SCREEN_RES_X) return;

	//Get relative ceil and floor heights
	float yceil = top_height - player->pos_height;
	float yfloor = bot_height - player->pos_height;

	//Project and get pixel position, like above.

	float proj_y0_ceiling = yceil * yscale0;
	float proj_y0_floor = yfloor * yscale0;

	float proj_y1_ceiling = yceil * yscale1;
	float proj_y1_floor = yfloor * yscale1;

	int y0ceiling = SCREEN_RES_Y/2 - (int)(proj_y0_ceiling);
	int y0floor = SCREEN_RES_Y/2 - (int)(proj_y0_floor);

	int y1ceiling = SCREEN_RES_Y/2 - (int)(proj_y1_ceiling);
	int y1floor = SCREEN_RES_Y/2 - (int)(proj_y1_floor);

	int x_begin;
	int x_end;

	x_begin = max_int(x0, 0);
	x_end = min_int(x1, SCREEN_RES_X);

	for(int x = x_begin; x < x_end; x++)
	{
		float relative_x = (float)(x-x0)/(float)(x1-x0);

		int screen_y_ceil = relative_x * (y1ceiling - y0ceiling) + y0ceiling;
		int screen_y_floor = relative_x * (y1floor - y0floor) + y0floor;

		int c_screen_y_ceil = clamp_int(screen_y_ceil, SCREEN_RES_Y, 0);
		int c_screen_y_floor = clamp_int(screen_y_floor, SCREEN_RES_Y, 0);			

		G3D_draw_wall(	x, 
						c_screen_y_ceil, screen_y_ceil, 
						c_screen_y_floor, screen_y_floor, 
						x0, x1+1, t_u0, t_u1, transformed_pos_0.y, transformed_pos_1.y,
						texture_parameters, tint, 
						1.0f, 1.0f);

	}
}

void G3D_draw_sprite_wall_db (	VECTOR2 start_pos, VECTOR2 end_pos,
								float bot_height, float top_height,
								GFX_TEXTURE_PARAM texture_parameters,
								TINT tint)
{
	G3D_draw_sprite_wall(start_pos, end_pos, bot_height, top_height, texture_parameters, tint);
	G3D_draw_sprite_wall(end_pos, start_pos, bot_height, top_height, texture_parameters, tint);
}

void G3D_draw_visplane(	int screen_x, int visible_top, int visible_bot,
						int is_ceiling, float visplane_height, 
						GFX_TEXTURE_PARAM texture_parameters,
						TINT tint)
{
	VECTOR2 world_space;
	VECTOR2 relative_space;
	int is_visible = 1;

	if(is_ceiling && visplane_height < 0.) is_visible = 0;
	if((!is_ceiling) && visplane_height > 0.) is_visible = 0;

	if(is_visible)
	{
		for(int screen_y = visible_top; screen_y < visible_bot; screen_y++)
		{
			if(texture_parameters.parallax)
			{
				int offset = (player->facing)/(2.*PI) * SKYBOX_SIZE_X;
				GFX_set_pixel_from_texture(	screen,
											texture_parameters,
											screen_x, screen_y,
											screen_x + offset, screen_y);
			}
			else
			{
				relative_space = convert_ss_to_rs(main_camera, point2(screen_x, screen_y), visplane_height);
				world_space = convert_rs_to_ws(relative_space);

				GFX_set_pixel_from_texture_depth_tint(	screen,
														texture_parameters,
														screen_x, screen_y,
														(int)(world_space.x * 128.), (int)(world_space.y * 128.),
														relative_space.y, tint);
			
			}
		}
	}
}
