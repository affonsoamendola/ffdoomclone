#ifndef EFFECT_H
#define EFFECT_H

typedef enum EffectType
{
	EFFECT_BREATHE
} EffectType;

typedef struct EffectBreathe_
{
	EffectType type;
	float rate;
	float min;
	float max;
	float current_time;
} EffectBreathe;

typedef union Effect_
{
	EffectType type;
	EffectBreathe breathe;
} Effect;

void update_effect(Effect* effect, void* out);

static inline Effect create_effect_breathe(float min, float max, float rate)
{
	Effect effect;

	effect.breathe.type = EFFECT_BREATHE;
	effect.breathe.min = min;
	effect.breathe.max = max;
	effect.breathe.rate = rate;
	effect.breathe.current_time = 0.0f;

	return effect; 
}

#endif