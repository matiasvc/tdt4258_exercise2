#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <stdlib.h>

typedef struct
{
	int16_t right;
	int16_t left;
} AudioSample;

typedef enum
{
	 SINE,
	 SQUARE,
	 TRIANGE,
	 SAWTOOTH,
	 NOISE
} EffectType;

typedef enum
{
	INSTANT,
	EASE_IN,
	EASE_OUT
} TransitionType;

typedef struct
{
	EffectType type;

	float attackTime;
	float sustainTime;
	float decayTime;

	uint16_t volume;

	TransitionType volumeAttackTransition;
	TransitionType volumeDecayTransition;

	uint16_t attackFrequency;
	uint16_t sustainFrequency;
	uint16_t decayFrequency;

	TransitionType frequencyAttackTransition;
	TransitionType frequencyDecayTransition;
} AudioEffect;

void setupMixer();
AudioSample getNextSample();

#endif
