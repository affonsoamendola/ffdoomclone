#ifndef COMMANDS_H
#define COMMANDS_H

void COMMAND_ver();

void COMMAND_intro();

void COMMAND_obj();

void COMMAND_obj_add_vertex(int object_index, float vx, float vy, float vz);
void COMMAND_obj_get_vertex(int object_index, int vertex_index);

#endif