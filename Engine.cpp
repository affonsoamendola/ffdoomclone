/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#include <iostream>
#include <string>

#include "Engine.hpp"

#include "SDL2/SDL.h"

using namespace std;

Engine::Engine() 
	:	m_game_running(true),
		m_edit_mode(false),
		m_game_mode(false),
		m_current_frame_start(0),
		m_last_frame_end(0),
		m_fps(0.)
{
	cout << "Initializing Internal Console System..." << endl;

	//console = make_unique_ptr<ConsoleSystem>();
}	

int main()
{
}