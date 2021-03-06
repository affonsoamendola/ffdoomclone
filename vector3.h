#ifndef VECTOR3_H
#define VECTOR3_H

typedef struct _VECTOR3
{
	float x;
	float y;
	float z;

} VECTOR3;

VECTOR3 vector3(float x, float y, float z);
VECTOR3 sum_v3(VECTOR3 vector_a, VECTOR3 vector_b);
VECTOR3 neg_v3(VECTOR3 vector);
VECTOR3 sub_v3(VECTOR3 vector_a, VECTOR3 vector_b);

VECTOR3 scale_v3(VECTOR3 vector, float scalar);

VECTOR3 element_scale_v3(VECTOR3 vector, VECTOR3 scale);

float dot_v3(VECTOR3 vector_a, VECTOR3 vector_b);
VECTOR3 cross_v3(VECTOR3 vector_a, VECTOR3 vector_b);

#endif