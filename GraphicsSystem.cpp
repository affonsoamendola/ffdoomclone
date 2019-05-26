/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#include "GraphicsSystem.hpp"

GraphicsSystem::GraphicsSystem(	const std::unique_ptr<Engine>& t_engine,
								unsigned int t_res_x,
								unsigned int t_res_y)
{
	this->engine = t_engine;

	
}