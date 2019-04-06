#ifndef POINT2_H
#define POINT2_H

typedef struct _POINT2
{
	int x;
	int y;

} POINT2;

POINT2 point2(int x, int y);

POINT2 sum_p2(POINT2 point_a, POINT2 point_b);
POINT2 sub_p2(POINT2 point_a, POINT2 point_b);
POINT2 neg_p2(POINT2 point);

POINT2 scale_p2(POINT2 point, float scalar);

#endif