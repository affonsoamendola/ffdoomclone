
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine.h"

#include "console.h"
#include "vector3.h"
#include "object.h"

char buffer[128];

void COMMAND_ver()
{
	CONSOLE_print("\nRT76800");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
}

void COMMAND_intro()
{
	CONSOLE_print("\n----------------------------------------");
	CONSOLE_print("\nFofonso's Raytracing Engine\n");
	CONSOLE_print("\nCodename RT76800");
	CONSOLE_print("\nCopyright Affonso Amendola, 2019");	
	CONSOLE_print("\nVersion ");
	CONSOLE_print(ENGINE_version());
	CONSOLE_print("\n\nBe Excellent to Each Other");
	CONSOLE_print("\n----------------------------------------");
}

void COMMAND_obj()
{

	object();
}

void COMMAND_obj_add_vertex(int object_index, float vx, float vy, float vz)
{

	object_add_vertex(get_object(object_index), vector3(vx, vy, vz));
}

void COMMAND_obj_get_vertex(int object_index, int vertex_index)
{
	OBJECT * object;

	object = get_object(object_index);

	CONSOLE_print("\nOBJECT INDEX ");
	sprintf(buffer, "%d", object_index);
	CONSOLE_print(buffer);

	if(vertex_index <= object->n_verts)
	{
		CONSOLE_print("\nX : ");
		sprintf(buffer, "%f", (*(get_object(object_index)->verts + vertex_index)).x);
		CONSOLE_print(buffer);

		CONSOLE_print("\nY : ");
		sprintf(buffer, "%f", (*(get_object(object_index)->verts + vertex_index)).y);
		CONSOLE_print(buffer);

		CONSOLE_print("\nZ : ");
		sprintf(buffer, "%f", (*(get_object(object_index)->verts + vertex_index)).y);
		CONSOLE_print(buffer);
	}
	else
	{
		CONSOLE_print("\nTHIS OBJECT DOES NOT HAVE THAT MANY VERTEXES");
	}
}
