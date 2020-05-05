#ifndef EFFECT_H
#define EFFECT_H

typedef enum EffectType
{
	EFFECT_BREATHE,
	EFFECT_RAINBOW
} EffectType;

typedef struct EffectBreathe_
{
	EffectType type;
	float rate;
	float min;
	float max;
	float current_time;
} EffectBreathe;

typedef struct EffectRainbow_
{
	EffectType type;
	float rate;
	unsigned char alpha;
	float current_time;
} EffectRainbow;

typedef union Effect_
{
	EffectType type;
	EffectBreathe breathe;
	EffectRainbow rainbow;
} Effect;

void update_effect(Effect* effect, void* out);
void get_effect(Effect* effect, void* in_data, void* out_data);

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

static inline Effect create_effect_rainbow(float rate, unsigned char alpha)
{
	Effect effect;

	effect.rainbow.type = EFFECT_RAINBOW;
	effect.rainbow.rate = rate;
	effect.rainbow.current_time = 0.0f;
	effect.rainbow.alpha = alpha;

	return effect; 
}


#endif