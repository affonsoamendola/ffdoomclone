#ifndef OBJECT_H
#define OBJECT_H

#include "vector3.h"

typedef struct OBJECT_
{
	int n_verts;
	VECTOR3 * verts;
	int n_tris;
	int * tris;
	VECTOR3 * normals;
}
OBJECT;

OBJECT * object();

OBJECT * get_object(int obj_index);

void object_add_vertex(OBJECT * object, VECTOR3 vertex);
void object_add_tri(OBJECT * object, int t1, int t2, int t3);

#endif