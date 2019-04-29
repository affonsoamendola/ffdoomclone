#include "point2.h"

POINT2 point2(int x, int y)
{
	POINT2 new_point;

	new_point.x = x;
	new_point.y = y;

	return new_point;
}

POINT2 sum_p2(POINT2 point_a, POINT2 point_b)
{
	POINT2 new_point;

	new_point.x = point_a.x + point_b.x;
	new_point.y = point_a.y + point_b.y;

	return new_point;
}

POINT2 sub_p2(POINT2 point_a, POINT2 point_b)
{
	POINT2 new_point;

	new_point.x = point_a.x - point_b.x;
	new_point.y = point_a.y - point_b.y;

	return new_point;
}

POINT2 neg_p2(POINT2 point)
{
	POINT2 new_point;

	new_point.x = -point.x;
	new_point.y = -point.y;

	return new_point;
}

POINT2 scale_p2(POINT2 point, float scalar)
{
	POINT2 new_point;

	new_point.x = (int)(((float)point.x) * scalar);
	new_point.y = (int)(((float)point.y) * scalar);

	return new_point;
}

void swap_p2(POINT2* a, POINT2* b)
{
	POINT2 temp;

	temp = *a;
	*a = *b;
	*b = temp;
}