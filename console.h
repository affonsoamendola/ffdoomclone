#ifndef CONSOLE_H
#define CONSOLE_H

#include "ff_stb.h"
#include "ff_color.h"

#include "effect.h"

#define CONSOLE_HISTORY_SIZE 16
#define CONSOLE_CHAR_LIMIT 256

typedef struct Console_
{
	bool open;
	char history[CONSOLE_HISTORY_SIZE][CONSOLE_CHAR_LIMIT];

	uint32_t font;
	uint32_t size_y;

	Color background_color;
	Color history_text_color;
	Color entry_color;
	Color entry_text_color;

	Effect rainbow_effect;
} Console;

extern Console console;

typedef struct ConsoleCommand_
{
	char* command;
	int arg_num;
	void (*function) (char** tokens);
} ConsoleCommand;

void toggle_console(void * console_);
void set_console_open(bool value);
bool is_console_open();

char* get_console_history(int history_index);
void set_console_font(uint32_t font);

void init_console();

void draw_console();

void scroll_console(int lines);
void printf_console(const char * char_string, ...);

void parse_console(const char* text_input);
void enter_console(void* null);

#endif