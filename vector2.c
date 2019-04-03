VECTOR2 vector2(float x, float y)
{
	VECTOR2 new_vector;

	new_vector.x = x;
	new_vector.y = y;

	return new_vector;	
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

	new_vector.x = -vector_a.x;
	new_vector.y = -vector_b.y;

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
	return (vector_a.x * vector_b.x) + (vector_a.y * vector_b.y)
}

float cross_v2(VECTOR2 vector_a, VECTOR2 vector_b)
{
	return (vector_a.x * vector_b.y) - (vector_a.y * vector_b.x);
}
