#ifndef CONSOLE_H
#define CONSOLE_H

#include "engine.h"

#define HISTORY_SIZE 16
#define CONSOLE_CHAR_LIMIT 256

void set_console_open(bool value);
bool get_console_open();

char* get_console_history(int history_index);

void CONSOLE_Init();

void parse_console(char* text_input);


#endif