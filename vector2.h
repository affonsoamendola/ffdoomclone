typedef struct _VECTOR2
{
	float x;
	float y;

} VECTOR2;

VECTOR2 vector2(float x, float y);
VECTOR2 sum_v2(VECTOR2 vector_a, VECTOR2 vector_b);
VECTOR2 neg_v2(VECTOR2 vector);
VECTOR2 sub_v2(VECTOR2 vector_a, VECTOR2 vector_b);

VECTOR2 scale_v2(VECTOR2 vector, float scalar);

float dot_v2(VECTOR2 vector_a, VECTOR2 vector_b);
float cross_v2(VECTOR2 vector_a, VECTOR2 vector_b);