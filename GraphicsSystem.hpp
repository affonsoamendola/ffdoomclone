/* 	
	Copyright Affonso Amendola 2019

	Distributed under GPLv3, use it to your hearts content,
	just remember the number one rule:

	Be Excellent to Each Other.
*/

#include <memory>

typedef struct SDL_Surface SDL_Surface;

class GraphicsSystem
{
private: 
	std::unique_ptr<Engine> engine;

	SDL_Surface * m_screen_surface;
	
	unsigned int m_pixel_scale;
	unsigned int m_res_x;
	unsigned int m_res_y;

	double m_aspect_ratio;

public:
	GraphicsSystem(	std::unique_ptr<Engine> t_engine,
					unsigned int t_res_x, 
					unsigned int t_res_y);
	~GraphicsSystem();

	void tick();
};