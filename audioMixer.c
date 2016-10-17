#include <stdlib.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"

#include "fixedptc.h"


#define EFFECT0_SAMPLES 4000
AudioEffect effect0 = {
	SINE, // Type
	100, // Attack Time
	2000, // Sustain Time
	300, // Decay Time
	100, // Attack Frequency
	3000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT1_SAMPLES 4000
AudioEffect effect1 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT2_SAMPLES 4000
AudioEffect effect2 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT3_SAMPLES 4000
AudioEffect effect3 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT4_SAMPLES 4000
AudioEffect effect4 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT5_SAMPLES 4000
AudioEffect effect5 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT6_SAMPLES 4000
AudioEffect effect6 = {
	SINE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT7_SAMPLES 4000
AudioEffect effect7 = {
	NOISE, // Type
	1000, // Attack Time
	2000, // Sustain Time
	1000, // Decay Time
	100, // Attack Frequency
	1000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};


#define SAMPLES_PER_SECOND 8000

#define RANDOM_BUFFER_LENGTH 1024
#define RANDOM_MAX 4294967295
static uint32_t randomBuffer[RANDOM_BUFFER_LENGTH];

bool isPlaying = false;
uint16_t playEffectLength = 0;
uint16_t playEffectReadCursor = 0;
int16_t *playEffectBufferPtr;

static int16_t sample0Buffer[EFFECT0_SAMPLES];
static int16_t sample1Buffer[EFFECT1_SAMPLES];
static int16_t sample2Buffer[EFFECT2_SAMPLES];
static int16_t sample3Buffer[EFFECT3_SAMPLES];
static int16_t sample4Buffer[EFFECT4_SAMPLES];
static int16_t sample5Buffer[EFFECT5_SAMPLES];
static int16_t sample6Buffer[EFFECT6_SAMPLES];
static int16_t sample7Buffer[EFFECT7_SAMPLES];

void fillRandomBuffer()
{
	uint32_t random = 879186745;

	for (int i = 0; i < RANDOM_BUFFER_LENGTH; ++i)
	{
		random = (1103515245 * random + 12345) % RANDOM_MAX;
		randomBuffer[i] = random;
	}
}

fixedpt squareWave(fixedpt t)
{
	fixedpt value = fixedpt_sin(fixedpt_mul(t, FIXEDPT_TWO_PI));
	value = fixedpt_mul(value, FIXEDPT_ONE_HALF) + FIXEDPT_ONE;
	int32_t squareValue = fixedpt_toint(value)*2 - 1;
	return fixedpt_fromint(squareValue);
}

fixedpt sineWave(fixedpt t)
{
	return fixedpt_sin(fixedpt_mul(t, FIXEDPT_TWO_PI));
}

fixedpt sawtoothWave(fixedpt t)
{
	fixedpt value = t & 0x0000FFFF;
	return fixedpt_mul(value, FIXEDPT_TWO) - FIXEDPT_ONE;
}

fixedpt noiseWave(fixedpt t)
{
	uint32_t index = fixedpt_toint(t) % RANDOM_BUFFER_LENGTH;
	fixedpt value = fixedpt_div(fixedpt_fromint(index), fixedpt_fromint(RANDOM_MAX));
	return fixedpt_mul(value, FIXEDPT_TWO) - FIXEDPT_ONE;
}


fixedpt easeInQuad(fixedpt start, fixedpt end, fixedpt value)
{
	end = end - start;
	fixedpt valueSqrd = fixedpt_mul(value, value);
	return fixedpt_mul(end, valueSqrd) + start;
}

fixedpt easeOutQuad(fixedpt start, fixedpt end, fixedpt value)
{
	end = end - start;
	end = fixedpt_mul(end, fixedpt_fromint(-1));
	value = fixedpt_mul(value, value - FIXEDPT_TWO);
	return fixedpt_mul(end, value) + start;
}


void renderEffects()
{	
	for (uint8_t effectIndex = 0; effectIndex < 8; effectIndex++)
	{		

		int16_t *sampleBuffer;
		AudioEffect effect;

		switch(effectIndex) // This would have been a lot prettier if malloc worked :(
		{
			case 0: { sampleBuffer = sample0Buffer; effect = effect0; } break;
			case 1: { sampleBuffer = sample1Buffer; effect = effect1; } break;
			case 2: { sampleBuffer = sample2Buffer; effect = effect2; } break;
			case 3: { sampleBuffer = sample3Buffer; effect = effect3; } break;
			case 4: { sampleBuffer = sample4Buffer; effect = effect4; } break;
			case 5: { sampleBuffer = sample5Buffer; effect = effect5; } break;
			case 6: { sampleBuffer = sample6Buffer; effect = effect6; } break;
			case 7: { sampleBuffer = sample7Buffer; effect = effect7; } break;
		}

		fixedpt (*waveFuncPointer)(fixedpt);

		fixedpt (*frequenceAttackFuncPointer)(fixedpt, fixedpt, fixedpt);
		fixedpt (*frequenceDecayFuncPointer)(fixedpt, fixedpt, fixedpt);

		switch (effect.type)
		{
			case SINE : { waveFuncPointer = &sineWave; } break;
			case SQUARE : { waveFuncPointer = &squareWave; } break;
			case SAWTOOTH : { waveFuncPointer = &sawtoothWave; } break;
			case NOISE : { waveFuncPointer = &noiseWave; } break;
		}

		switch (effect.frequencyAttackTransition)
		{
			case EASE_IN : { frequenceAttackFuncPointer = &easeInQuad; } break;
			case EASE_OUT : { frequenceAttackFuncPointer = &easeOutQuad; } break;
		}

		switch (effect.frequencyDecayTransition)
		{
			case EASE_IN : { frequenceDecayFuncPointer = &easeInQuad; } break;
			case EASE_OUT : { frequenceDecayFuncPointer = &easeOutQuad; } break;
		}

		fixedpt attakStartDt = fixedpt_div(fixedpt_fromint(effect.attackFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));
		fixedpt sustainDt = fixedpt_div(fixedpt_fromint(effect.sustainFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));
		fixedpt decayEndDt = fixedpt_div(fixedpt_fromint(effect.decayFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));

		fixedpt t = 0;
		fixedpt dt = 0;

		uint16_t start = 0;
		uint16_t end = effect.attackTime;
		for (uint16_t i = start; i < end; ++i)
		{
			fixedpt lerp = fixedpt_div(fixedpt_fromint(i), fixedpt_fromint(effect.attackTime) - FIXEDPT_ONE);
			dt = (*frequenceAttackFuncPointer)(attakStartDt, sustainDt, lerp);

			fixedpt fpvalue = (*waveFuncPointer)(t);
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));
			sampleBuffer++;
			t += dt;
		}
		
		dt = sustainDt;

		start = effect.attackTime;
		end = effect.attackTime + effect.sustainTime;
		for (uint16_t i = start; i < end; ++i)
		{

			fixedpt fpvalue = (*waveFuncPointer)(t);
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));
			sampleBuffer++;
			t += dt;
		}

		start = effect.attackTime + effect.sustainTime;
		end = effect.attackTime + effect.sustainTime + effect.decayTime;
		for (uint16_t i = start; i < end; ++i)
		{
			fixedpt lerp = fixedpt_div(fixedpt_fromint(i) - fixedpt_fromint(start), fixedpt_fromint(end - start));
			dt = (*frequenceDecayFuncPointer)(sustainDt, decayEndDt, lerp);

			fixedpt fpvalue = (*waveFuncPointer)(t);
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));
			sampleBuffer++;
			t += dt;
		}
	}
}


void setupMixer()
{
	fillRandomBuffer();
	renderEffects();
}

void playEffect(uint8_t index)
{
	isPlaying = true;
	playEffectReadCursor = 0;
	switch(index)
	{
		case 0: { playEffectLength = EFFECT0_SAMPLES; playEffectBufferPtr = sample0Buffer; } break;
		case 1: { playEffectLength = EFFECT1_SAMPLES; playEffectBufferPtr = sample1Buffer; } break;
		case 2: { playEffectLength = EFFECT2_SAMPLES; playEffectBufferPtr = sample2Buffer; } break;
		case 3: { playEffectLength = EFFECT3_SAMPLES; playEffectBufferPtr = sample3Buffer; } break;
		case 4: { playEffectLength = EFFECT4_SAMPLES; playEffectBufferPtr = sample4Buffer; } break;
		case 5: { playEffectLength = EFFECT5_SAMPLES; playEffectBufferPtr = sample5Buffer; } break;
		case 6: { playEffectLength = EFFECT6_SAMPLES; playEffectBufferPtr = sample6Buffer; } break;
		case 7: { playEffectLength = EFFECT7_SAMPLES; playEffectBufferPtr = sample7Buffer; } break;		
	}
}

int16_t getNextSample()
{
	uint16_t sample = 0;

	if (isPlaying)
	{
		sample = *playEffectBufferPtr;
		playEffectBufferPtr++;
		playEffectReadCursor++;

		if (playEffectReadCursor >= playEffectLength)
		{
			isPlaying = false;
		}
	}

	return sample;
}
