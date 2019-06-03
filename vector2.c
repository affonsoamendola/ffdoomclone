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

float norm_v2(VECTOR2 vector)
{
	float norm_squared = dot_v2(vector, vector);

	return sqrt(norm_squared);
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

float distance_v2_to_segment(VECTOR2 point, VECTOR2 start, VECTOR2 end, VECTOR2 * closest_point)
{
	float distance;

	VECTOR2 v;
	VECTOR2 w;

	float c1, c2;

	v = sub_v2 (end, start);
	w = sub_v2 (point, start);

	c1 = dot_v2(v, w);

	if(c1 <= 0)
	{
		if(closest_point != NULL)
			*closest_point = start;
		return norm_v2(sub_v2(point, start));
	}

	c2 = dot_v2(v, v);

	if(c2 <= c1)
	{
		if(closest_point != NULL)
			*closest_point = end;
		return norm_v2(sub_v2(point, end));
	}

	float p = c1/c2;

	VECTOR2 projection;

	projection = sum_v2(start, scale_v2(v, p));

	if(closest_point != NULL)
		*closest_point = projection;

	return norm_v2(sub_v2(projection, point));			
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

int intersect_check_v2(VECTOR2 v1, VECTOR2 v2, VECTOR2 v3, VECTOR2 v4, VECTOR2 * intersect_vector)
{
	double denominator = ((v4.x - v3.x) * (v1.y - v2.y) - (v1.x - v2.x) * (v4.y - v3.y));

	if(denominator <= EPSILON && denominator >= -EPSILON) return -1;

	VECTOR2 len_segment_1;

	double t_a = 	((v3.y - v4.y) * (v1.x - v3.x) + (v4.x - v3.x) * (v1.y - v3.y)) / denominator;

	//Long live Total Biscuit, we really miss you, 
	//wish you could review my shitty game... maybe in another life.
	double t_b = 	((v1.y - v2.y) * (v1.x - v3.x) + (v2.x - v1.x) * (v1.y - v3.y)) / denominator;

	if(t_a >= 0. && t_a <= 1. && t_b >= 0. && t_b <= 1.)
	{
		if(intersect_vector != NULL)
		{
			len_segment_1 = sub_v2(v2, v1);

			*intersect_vector = sum_v2(v1, scale_v2(len_segment_1, (float)t_a));
		}

		return 1;
	}
	else return 0;
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
