#ifndef COMMANDS_H
#define COMMANDS_H

void command_ver(char** tokens);

void command_intro(char** tokens);

void command_help(char** tokens);

void command_sector_show(char** tokens);

void command_vertex(char** tokens);
void command_vertex_list(char** tokens);
void command_remove_vertex(char** tokens);

void command_noclip(char** tokens);

void command_set_tint(char** tokens);

void command_set(char** tokens);
void command_save_level(char** tokens);
void command_load_level(char** tokens);

void command_edit();

#endif