#ifndef VECTOR2_H
#define VECTOR2_H

#include "point2.h"

typedef struct _VECTOR2
{
	float x;
	float y;

} VECTOR2;

VECTOR2 vector2(float x, float y);
VECTOR2 p2v2(POINT2 point);

VECTOR2 sum_v2(VECTOR2 vector_a, VECTOR2 vector_b);
VECTOR2 neg_v2(VECTOR2 vector);
VECTOR2 sub_v2(VECTOR2 vector_a, VECTOR2 vector_b);

VECTOR2 scale_v2(VECTOR2 vector, float scalar);

VECTOR2 rot_v2(VECTOR2 vector, float ang_rad);

float dot_v2(VECTOR2 vector_a, VECTOR2 vector_b);
float cross_v2(VECTOR2 vector_a, VECTOR2 vector_b);

float norm_v2(VECTOR2 vector);

VECTOR2 intersect_v2(VECTOR2 start_a, VECTOR2 end_a, VECTOR2 start_b, VECTOR2 end_b);
int intersect_box_v2(VECTOR2 box_a_start, VECTOR2 box_a_end, VECTOR2 box_b_start, VECTOR2 box_b_end);
int point_side_v2(VECTOR2 point, VECTOR2 v0, VECTOR2 v1);

void swap_v2(VECTOR2* a, VECTOR2* b);

#endif