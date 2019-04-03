VECTOR3 vector3(float x, float y, float z)
{
	VECTOR3 new_vector;

	new_vector.x = x;
	new_vector.y = y;
	new_vector.z = z;

	return new_vector;
}

VECTOR3 sum_v3(VECTOR3 vector_a, VECTOR3 vector_b)
{
	VECTOR3 new_vector;

	new_vector.x = vector_a.x + vector_b.x;
	new_vector.y = vector_a.y + vector_b.y;
	new_vector.z = vector_a.z + vector_b.z;

	return new_vector;
}

VECTOR3 neg_v3(VECTOR3 vector)
{
	VECTOR3 new_vector;

	new_vector.x = -vector.x;
	new_vector.y = -vector.y;
	new_vector.z = -vector.z;

	return new_vector;
}

VECTOR3 sub_v3(VECTOR3 vector_a, VECTOR3 vector_b);
{
	VECTOR3 new_vector;

	new_vector.x = vector_a.x - vector_b.x;
	new_vector.y = vector_a.y - vector_b.y;
	new_vector.z = vector_a.z - vector_b.z;

	return new_vector;
}

VECTOR3 scale_v3(VECTOR3 vector, float scalar);
{
	VECTOR3 new_vector;

	new_vector.x = vector.x * scalar;
	new_vector.y = vector.y * scalar;
	new_vector.z = vector.z * scalar;

	return new_vector;
}

float dot_v3(VECTOR3 vector_a, VECTOR3 vector_b)
{
	return (vector_a.x*vector_b.x) + (vector_a.y*vector_b.y) + (vector_a.z*vector_b.z);
}

VECTOR3 cross_v3(VECTOR3 vector_a, VECTOR3 vector_b)
{
	VECTOR3 new_vector;

	new_vector.x = (vector_a.y*vector_b.z) - (vector_a.z*vector_b.y);
	new_vector.y = (vector_a.z*vector_b.x) - (vector_a.x*vector_b.z);
	new_vector.z = (vector_a.x*vector_b.y) - (vector_a.y*vector_b.x);

	return new_vector;
}
