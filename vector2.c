#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include "utility.h"

#include "engine.h"

#include "vector2.h"
#include "point2.h"

VECTOR2 vector2(float x, float y)
{
	VECTOR2 new_vector;

	new_vector.x = x;
	new_vector.y = y;

	return new_vector;	
}

VECTOR2 p2v2(POINT2 point)
{
	VECTOR2 vector;

	vector.x = (float)point.x;
	vector.y = (float)point.y;

	return vector;
}

VECTOR2 sum_v2(VECTOR2 vector_a, VECTOR2 vector_b)
{
	VECTOR2 new_vector;

	new_vector.x = vector_a.x + vector_b.x;
	new_vector.y = vector_a.y + vector_b.y;

	return new_vector;
}

VECTOR2 neg_v2(VECTOR2 vector)
{
	VECTOR2 new_vector;

	new_vector.x = -vector.x;
	new_vector.y = -vector.y;

	return new_vector;
}

VECTOR2 sub_v2(VECTOR2 vector_a, VECTOR2 vector_b)
{
	VECTOR2 new_vector;

	new_vector.x = vector_a.x - vector_b.x;
	new_vector.y = vector_a.y - vector_b.y;

	return new_vector;
}

VECTOR2 scale_v2(VECTOR2 vector, float scalar)
{
	VECTOR2 new_vector;

	new_vector.x = vector.x * scalar;
	new_vector.y = vector.y * scalar;

	return new_vector;
}

float dot_v2(VECTOR2 vector_a, VECTOR2 vector_b)
{
	return (vector_a.x * vector_b.x) + (vector_a.y * vector_b.y);
}

float cross_v2(VECTOR2 vector_a, VECTOR2 vector_b)
{
	return (vector_a.x * vector_b.y) - (vector_a.y * vector_b.x);
}

VECTOR2 rot_v2(VECTOR2 vector, float ang_rad)
{
	VECTOR2 new_vector;

	new_vector.x = vector.x * cos(ang_rad) - vector.y * sin(ang_rad);
	new_vector.y = vector.x * sin(ang_rad) + vector.y * cos(ang_rad);

	return new_vector;
}


VECTOR2 intersect_v2(VECTOR2 start_a, VECTOR2 end_a, VECTOR2 start_b, VECTOR2 end_b)
{
	VECTOR2 new_vector;

	new_vector.x = 	((start_a.x * end_a.y - start_a.y * end_a.x)*(start_b.x - end_b.x) - (start_a.x - end_a.x)*(start_b.x * end_b.y - start_b.y*end_b.x)) /
				 	((start_a.x - end_a.x) * (start_b.y - end_b.y) - (start_a.y - end_a.y)*(start_b.x - end_b.x));

	new_vector.y = 	((start_a.x * end_a.y - start_a.y * end_a.x)*(start_b.y - end_b.y) - (start_a.y - end_a.y)*(start_b.x * end_b.y - start_b.y*end_b.x)) /
				 	((start_a.x - end_a.x) * (start_b.y - end_b.y) - (start_a.y - end_a.y)*(start_b.x - end_b.x));


	return new_vector;
}

int point_side_v2(VECTOR2 point, VECTOR2 v0, VECTOR2 v1)
{
	float cross;

	cross = cross_v2(sub_v2(v1, v0), sub_v2(point, v0));

	if(cross > EPSILON)
		return 1;

	if(cross < -EPSILON)
		return -1;

	return 0;
}

int intersect_box_v2(VECTOR2 box_a_start, VECTOR2 box_a_end, VECTOR2 box_b_start, VECTOR2 box_b_end)
{
	if(	(min_float(box_a_start.x, box_a_end.x) <= max_float(box_b_start.x, box_b_end.x) && min_float(box_b_start.x, box_b_end.x) <= max_float(box_a_start.x, box_a_end.x)) &&
		(min_float(box_a_start.y, box_a_end.y) <= max_float(box_b_start.y, box_b_end.y) && min_float(box_b_start.y, box_b_end.y) <= max_float(box_a_start.y, box_a_end.y)) )
		return 1;
	else
		return 0;
}

void swap_v2(VECTOR2* a, VECTOR2* b)
{
	VECTOR2 temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
