#ifndef OBJECT_H
#define OBJECT_H

typedef struct OBJECT_
{
	VECTOR3 * vertexes;
	int * triangles;
	VECTOR3 * normals;
}
OBJECT;

#endif