#ifndef ANIM_H
#define ANIM_H

typedef struct _ANIM
{
	int frame_count;

	int * frame_timings;

	int current_tick;

	GFX_TEXTURE ** frames;

}ANIM;

void CREATE_Animation(ANIM * animation, char* anim_file_definition);
void TICK_Animation(ANIM * animation);
GFX_TEXTURE * ANIM_Get_Frame(ANIM * animation, int index);
GFX_TEXTURE * ANIM_Get_Current_Frame(ANIM * animation);

#endif