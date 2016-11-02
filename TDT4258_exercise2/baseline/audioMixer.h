#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <stdlib.h>
#include "fixedptc.h"

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
	
	uint16_t volume;

	uint16_t attackTime;
	uint16_t sustainTime;
	uint16_t decayTime;

 
	uint16_t attackFrequency;
	uint16_t sustainFrequency;
	uint16_t decayFrequency;

	TransitionType frequencyAttackTransition;
	TransitionType frequencyDecayTransition;
} AudioEffect;

void setupMixer();
void playEffect(uint8_t index);
void playMelody();
int16_t getNextSample();

#endif
