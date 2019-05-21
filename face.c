#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#include "gfx.h"

#include "face.h"

GFX_TEXTURE face_sheet;

int current_anim;
int current_state;

enum FACE_ANIM
{
	ANIM_IDLE 			= 0,
	ANIM_MANIAC			= 1
};

#define IDLE_C 			(SDL_Rect){0, 	0, 38, 38}
#define IDLE_L 			(SDL_Rect){38, 	0, 38, 38}
#define IDLE_R 			(SDL_Rect){76, 	0, 38, 38}
#define IDLE_BLINK 		(SDL_Rect){114,  0, 38, 38}
#define MANIAC 			(SDL_Rect){152,  0, 38, 38}

int anim_counter = 0;

SDL_Rect current_rect;

extern SDL_Surface * screen;


void FACE_Init()
{
	current_anim = ANIM_IDLE;
	current_rect = IDLE_C;
	current_state = 0;
	GFX_load_texture_at("graphix/flau_face.png", &face_sheet);
}

void FACE_Tick()
{
	switch(current_anim)
	{
		case ANIM_IDLE:
			FACE_ANIM_Idle();
			break;

		case ANIM_MANIAC:
			//FACE_ANIM_Maniac();
			break;
	}
	anim_counter++;
}

void FACE_Draw()
{
	GFX_blit(face_sheet.surface, screen, current_rect, point2(142,201), GFX_Tint(1., 1., 1.));
}

#define ANIM_IDLE_0 10
#define ANIM_IDLE_1 20
#define ANIM_IDLE_2 20
#define ANIM_IDLE_3 5
#define ANIM_IDLE_4 1

#define ANIM_IDLE_RAND 40
#define ANIM_IDLE_BLINK_RAND 2

int random_variation = 0;

void FACE_ANIM_Idle()
{
	switch(current_state)
	{
		case 0:
			current_rect = IDLE_C;
			if(anim_counter > ANIM_IDLE_0 + random_variation) {anim_counter = 0; current_state = 1; random_variation = rand()%ANIM_IDLE_RAND;}
			break;

		case 1:
			current_rect = IDLE_L;
			if(anim_counter > ANIM_IDLE_1 + random_variation) {anim_counter = 0; current_state = 2; random_variation = rand()%ANIM_IDLE_RAND;}
			break;

		case 2:
			current_rect = IDLE_R;
			if(anim_counter > ANIM_IDLE_2 + random_variation) {anim_counter = 0; current_state = 3; random_variation = rand()%ANIM_IDLE_RAND;}
			break;

		case 3:
			current_rect = IDLE_C;
			if(anim_counter > ANIM_IDLE_3 + random_variation) 
			{
				anim_counter = 0; 
				if(rand()%2 == 1) {	current_state = 4; random_variation = rand()%ANIM_IDLE_BLINK_RAND;}
				else 			  { current_state = 0; random_variation = rand()%ANIM_IDLE_RAND;}
				
			}
			break;

		case 4:
			current_rect = IDLE_BLINK;
			if(anim_counter > ANIM_IDLE_4 + random_variation) {anim_counter = 0; current_state = 0; random_variation = rand()%ANIM_IDLE_RAND;}
			break;
	}
}