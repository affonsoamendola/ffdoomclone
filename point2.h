#ifndef POINT2_H
#define POINT2_H

#define ZERO_POINT2 (POINT2){0, 0}

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
void swap_p2(POINT2* a, POINT2* b);

POINT2 clamp_p2(POINT2 in, POINT2 min, POINT2 max);

#endif