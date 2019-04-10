#include <stdlib.h>
#include <stdio.h>

#include "vector3.h"
#include "list.h"

#include "object.h"

extern LIST * object_list;

OBJECT * object()
{
	OBJECT * new_object_pointer = malloc(sizeof(OBJECT));

	new_object_pointer->n_verts = 0;
	new_object_pointer->n_tris = 0;

	new_object_pointer->verts = NULL;
	new_object_pointer->tris = NULL;
	new_object_pointer->normals = NULL;

	append_list(object_list, new_object_pointer);

	return new_object_pointer;
}

OBJECT * get_object(int obj_index)
{

	return get_list_at(object_list, obj_index);
}

void object_add_vertex(OBJECT * object, VECTOR3 vertex)
{
	VECTOR3 * old_vertex_array;
	VECTOR3 * new_vertex_array;

	old_vertex_array = object->verts;
	new_vertex_array = malloc(((object->n_verts)+1) * sizeof(VECTOR3));

	for(int i = 0; i < object->n_verts; i ++)
	{
		*(new_vertex_array + i) = *(old_vertex_array + i); 
	}

	*(new_vertex_array + object->n_verts) = vertex;

	free(old_vertex_array);

	object->verts = new_vertex_array;
	object->n_verts = object->n_verts + 1;
}

void object_add_tri(OBJECT * object, int t1, int t2, int t3)
{
	int * old_tri_array;
	int * new_tri_array;

	old_tri_array = object->tris;
	new_tri_array = malloc(((object->n_tris)+1)*3 * sizeof(int));

	for(int i = 0; i < (object->n_tris*3); i ++)
	{
		*(new_tri_array + i) = *(old_tri_array + i); 
	}

	*(new_tri_array + (object->n_tris*3)) = t1;
	*(new_tri_array + (object->n_tris*3)+1) = t2;
	*(new_tri_array + (object->n_tris*3)+2) = t3;

	free(old_tri_array);

	object->tris = new_tri_array;
	object->n_tris = object->n_tris + 1;
}


