#ifndef CONSOLE_H
#define CONSOLE_H

#include "ff_stb.h"

#define CONSOLE_HISTORY_SIZE 16
#define CONSOLE_CHAR_LIMIT 256

typedef struct Console_
{
	bool open;
	int cursor_location;
	char history[CONSOLE_CHAR_LIMIT * CONSOLE_HISTORY_SIZE];

} Console;

typedef struct ConsoleCommand_
{
	char* command;
	int arg_num;
	void (*function) (char** tokens);
} ConsoleCommand;

void set_console_open(bool value);
bool is_console_open();

char* get_console_history(int history_index);

Console* console_init();

void scroll_console(int lines);
void printf_console(const char * char_string, ...);

void parse_console(const char* text_input);



#endif