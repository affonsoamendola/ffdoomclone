#ifndef _3D_H
#define _3D_H

#include "camera.h"
#include "ff_vector2.h"

static inline Vector2f transform_camera(const Camera* cam, const Vector2f world_pos)
{
	Vector2f transformed;

	transformed = sub_v2(world_pos, cam->position);
	transformed = rot_v2(transformed, cam->facing);

	return transformed;
}

static inline float rel_height_camera(const Camera* cam, const float world_height)
{

	return world_height - cam->height;
}

static inline void project_cam(	const Camera* cam, const Vector2f rel_pos,
								float* scale)
{
	*scale = cam->proj / rel_pos.y; //y in the relative position means depth.
}

void render_wall(	Camera* cam,
					const Vector2f v0_world, const Vector2f v1_world, 
					float floor_world, float ceiling_world);

void render_level(Camera* cam);

#endif 