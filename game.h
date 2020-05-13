#ifndef _GAME_H
#define _GAME_H

typedef struct Game_
{
	char padding;
}Game;

extern Game game;

void init_game();
void update_game();
void draw_game();
void quit_game();

#endif