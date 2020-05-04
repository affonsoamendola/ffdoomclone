#include "effect.h"
#include "engine.h"

#include "ff_math.h"

void update_breathe_effect(EffectBreathe* effect, float* out)
{
	effect->current_time += engine_delta_time()*(effect->rate*M_PI*2.0);

	while(effect->current_time > (M_PI*2.0)) effect->current_time -= (M_PI*2.0);

	*out = ff_sin_lim_f(effect->current_time, effect->min, effect->max);
}

void update_effect(Effect* effect, void* out)
{
	switch(effect->type)
	{
		case(EFFECT_BREATHE):
			update_breathe_effect(&(effect->breathe), (float*)out);
			break;
	}
}

