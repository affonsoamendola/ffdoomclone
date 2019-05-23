#ifndef COMMANDS_H
#define COMMANDS_H

void COMMAND_ver();

void COMMAND_intro();

void COMMAND_sector_show(int sector_index);
void COMMAND_vertex_list();

void COMMAND_noclip();

void COMMAND_set_tint(float r, float g, float b);

void COMMAND_set(char * variable, int value);
void COMMAND_save_level(char * filename);
void COMMAND_load_level(char * filename);

#endif