/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/
#pragma once
#include <memory>

class Engine
{
private:
	bool m_game_running;
	bool m_edit_mode;
	bool m_game_mode;

	clock_t m_current_frame_start;
	clock_t m_last_frame_end;

	double m_fps;

/*	std::unique_ptr<ConsoleSystem> console;
	std::unique_ptr<GraphicsSystem> graphics;
	std::unique_ptr<InputSystem> input;
	std::unique_ptr<GameWorld> world;
*/
public:
	Engine();
	~Engine();

	double deltaTime();

	void update();
	void tick();

	std::string version();
};