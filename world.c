
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#include "world.h"
#include "console.h"

LIST * object_list;

void WORLD_Init()
{
	CONSOLE_print("Initting World... Please hold...");

	object_list = create_list();
}