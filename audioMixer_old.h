#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <stdlib.h>
#include "fixedptc.h"

typedef struct
{
	int16_t right;
	int16_t left;
} AudioSample;

typedef enum
{
	 SINE,
	 SQUARE,
	 SAWTOOTH,
	 NOISE
} EffectType;

typedef enum
{
	EASE_IN,
	EASE_OUT
} TransitionType;

typedef struct
{
	EffectType type;

	float attackTime;
	float sustainTime;
	float decayTime;

	uint16_t attackVolume;
	uint16_t sustainVolume;
	uint16_t decayVolume;

	TransitionType volumeAttackTransition;
	TransitionType volumeDecayTransition;
 
	uint16_t attackFrequency;
	uint16_t sustainFrequency;
	uint16_t decayFrequency;

	TransitionType frequencyAttackTransition;
	TransitionType frequencyDecayTransition;
} AudioEffect;

void setupMixer();
void renderAudio();
AudioSample getNextSample();

#endif
