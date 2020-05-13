#ifndef CAMERA_H
#define CAMERA_H

#include "gfx.h"

typedef struct Camera_
{
	Vector2f position;
	
	float facing;
	float height;

	float near_clip;
	float far_clip;

	float aspect_ratio;
	float hor_fov;

	float near_x;
	float near_y;

	float far_x;
	float far_y;

	float proj;
} Camera;

static inline void init_camera(	Camera* cam, 
								const float near_clip,
								const float far_clip,
								const float aspect_ratio,
								const float hor_fov)
{
	cam->near_clip = near_clip;
	cam->far_clip = far_clip;

	cam->aspect_ratio = aspect_ratio;
	cam->hor_fov = hor_fov;

	cam->position = ZERO_VECTOR2F;
	cam->facing = 0.0f;
	cam->height = 1.0f;

	cam->near_x = tan(hor_fov) * near_clip;
	cam->near_y = cam->near_x / aspect_ratio;

	cam->far_x = tan(hor_fov) * far_clip;
	cam->far_y = cam->far_x / aspect_ratio;

	cam->proj = gfx.screen_res_x / (2.0f*tan(cam->hor_fov));
}

#endif