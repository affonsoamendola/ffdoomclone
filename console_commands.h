#ifndef COMMANDS_H
#define COMMANDS_H

void COMMAND_ver(char** tokens);

void COMMAND_intro(char** tokens);

void COMMAND_help(char** tokens);

void COMMAND_sector_show(char** tokens);
void COMMAND_vertex_list(char** tokens);

void COMMAND_noclip(char** tokens);

void COMMAND_set_tint(char** tokens);

void COMMAND_set(char** tokens);
void COMMAND_save_level(char** tokens);
void COMMAND_load_level(char** tokens);

void COMMAND_edit();

#endif