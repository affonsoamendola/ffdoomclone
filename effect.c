#include "effect.h"
#include "engine.h"

#include "ff_color.h"
#include "ff_math.h"

void update_breathe_effect(EffectBreathe* effect, float* out)
{
	effect->current_time += engine_delta_time()*(effect->rate*M_PI*2.0);
	while(effect->current_time > (M_PI*2.0)) effect->current_time -= (M_PI*2.0);

	*out = ff_sin_lim_f(effect->current_time, effect->min, effect->max);
}

void update_rainbow_effect(EffectRainbow* effect)
{
	effect->current_time += engine_delta_time()*(effect->rate*M_PI*2.0);
	while(effect->current_time > (M_PI*2.0)) effect->current_time -= (M_PI*2.0);
}

void update_effect(Effect* effect, void* out_data)
{
	switch(effect->type)
	{
		case(EFFECT_BREATHE):
			update_breathe_effect(&(effect->breathe), (float*)out_data);
			break;
		case(EFFECT_RAINBOW):
			update_rainbow_effect(&(effect->rainbow));
			break;
	}
}

void get_rainbow_effect(EffectRainbow* effect, float* offset_, Color* out_color)
{
	float offset = 0.0;
	if(offset_ != NULL) offset = *offset_;

	float r = ff_sin_lim_f(effect->current_time +                  offset, 0.0f, 255.0f);
	float g = ff_sin_lim_f(effect->current_time + (2.0/3.0)*M_PI + offset, 0.0f, 255.0f);
	float b = ff_sin_lim_f(effect->current_time + (4.0/3.0)*M_PI + offset, 0.0f, 255.0f);

	out_color->r = r;
	out_color->g = g;
	out_color->b = b;
	out_color->a = effect->alpha;
}

void get_effect(Effect* effect, void* in_data, void* out_data)
{
	switch(effect->type)
	{
		case(EFFECT_RAINBOW):
			get_rainbow_effect(&(effect->rainbow), (float*) in_data, (Color*) out_data);
	}
}


