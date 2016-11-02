#include <stdlib.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"

#include "fixedptc.h"


#define EFFECT0_SAMPLES 1500
AudioEffect effect0 = { // E7
	SINE, // Type
	15000, // Volume
	0, // Attack Time
	1000, // Sustain Time
	0, // Decay Time
	0, // Attack Frequency
	2637, // Sustain Frequency
	0, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT1_SAMPLES 1500
AudioEffect effect1 = { // C7
	SINE, // Type
	15000, // Volume
	0, // Attack Time
	1000, // Sustain Time
	0, // Decay Time
	0, // Attack Frequency
	2093, // Sustain Frequency
	0, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT2_SAMPLES 3000
AudioEffect effect2 = { // G7
	SINE, // Type
	15000, // Volume
	0, // Attack Time
	1000, // Sustain Time
	0, // Decay Time
	0, // Attack Frequency
	3136, // Sustain Frequency
	0, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT3_SAMPLES 1000
AudioEffect effect3 = { // G6
	SINE, // Type
	15000, // Volume
	0, // Attack Time
	1000, // Sustain Time
	0, // Decay Time
	0, // Attack Frequency
	1568, // Sustain Frequency
	0, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT4_SAMPLES 1500
AudioEffect effect4 = { // Shoot effect
	SINE, // Type
	15000, // Volume
	0, // Attack Time
	0, // Sustain Time
	1500, // Decay Time
	0, // Attack Frequency
	2000, // Sustain Frequency
	30, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT5_SAMPLES 7000
AudioEffect effect5 = { // Alert effect
	NOISE, // Type
	15000, // Volume
	2000, // Attack Time
	3000, // Sustain Time
	2000, // Decay Time
	200, // Attack Frequency
	800, // Sustain Frequency
	200, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT6_SAMPLES 800
AudioEffect effect6 = { // Enemy shoot
	SQUARE, // Type
	15000, // Volume
	200, // Attack Time
	0, // Sustain Time
	600, // Decay Time
	100, // Attack Frequency
	4000, // Sustain Frequency
	100, // Decay Frequency
	EASE_IN, // Frequency Attack Tranition
	EASE_OUT // Frequency Decay Transition
};

#define EFFECT7_SAMPLES 10000
AudioEffect effect7 = { // Death sound
	SAWTOOTH, // Type
	15000, // Volume
	0, // Attack Time
	0, // Sustain Time
	10000, // Decay Time
	100, // Attack Frequency
	6000, // Sustain Frequency
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

static uint8_t melodyIndex = 0;
static uint8_t melodySamples[] = {0, 0, 0, 1, 0, 2, 3};
bool playingMelody = true;

int16_t sample0Buffer[EFFECT0_SAMPLES];
int16_t sample1Buffer[EFFECT1_SAMPLES];
int16_t sample2Buffer[EFFECT2_SAMPLES];
int16_t sample3Buffer[EFFECT3_SAMPLES];
int16_t sample4Buffer[EFFECT4_SAMPLES];
int16_t sample5Buffer[EFFECT5_SAMPLES];
int16_t sample6Buffer[EFFECT6_SAMPLES];
int16_t sample7Buffer[EFFECT7_SAMPLES];

/**
	Fills the randomBuffer with pseudorandom numbers.
**/
void fillRandomBuffer()
{
	uint32_t random = 879186745;

	for (int i = 0; i < RANDOM_BUFFER_LENGTH; ++i)
	{
		random = (1103515245 * random + 12345) % RANDOM_MAX;
		randomBuffer[i] = random;
	}
}

/**
	Returns a squarewave between -1 and 1, with perior 1*t
	@param t	The time input for the function.
**/
fixedpt squareWave(fixedpt t)
{
	fixedpt value = fixedpt_sin(fixedpt_mul(t, FIXEDPT_TWO_PI));
	value = fixedpt_mul(value, FIXEDPT_ONE_HALF) + FIXEDPT_ONE;
	int32_t squareValue = fixedpt_toint(value)*2 - 1;
	return fixedpt_fromint(squareValue);
}

/**
	Returns a sinewave between -1 and 1, with perior 1*t
	@param t	The time input for the function.
**/
fixedpt sineWave(fixedpt t)
{
	return fixedpt_sin(fixedpt_mul(t, FIXEDPT_TWO_PI));
}

/**
	Returns a sawtooth between -1 and 1, with perior 1*t
	@param t	The time input for the function.
**/
fixedpt sawtoothWave(fixedpt t)
{
	fixedpt value = t & 0x0000FFFF;
	return fixedpt_mul(value, FIXEDPT_TWO) - FIXEDPT_ONE;
}

/**
	Returns a noisewave between -1 and 1, with perior 1*t
	@param t	The time input for the function.
**/
fixedpt noiseWave(fixedpt t)
{
	uint32_t index = fixedpt_toint(t) % RANDOM_BUFFER_LENGTH;
	#pragma GCC diagnostic ignored "-Woverflow" // Ignore overflow error, as this is by design
	fixedpt value = fixedpt_div(fixedpt_fromint(index), fixedpt_fromint(RANDOM_MAX));
	#pragma GCC diagnostic pop
	return fixedpt_mul(value, FIXEDPT_TWO) - FIXEDPT_ONE;
}

/**
	A easing function between start and end such that df(1)/dt = 0
	@param start	The starting value of the function.
	@param end		The ending value of the function.
	@param value	The input for the function, sould be between 0 and 1 inclusive.
**/
fixedpt easeInQuad(fixedpt start, fixedpt end, fixedpt value)
{
	end = end - start;
	fixedpt valueSqrd = fixedpt_mul(value, value);
	return fixedpt_mul(end, valueSqrd) + start;
}

/**
	A easing function between 0 and 1 such that df(0)/dt = 0
	@param start	The starting value of the function.
	@param end		The ending value of the function.
	@param value	The input for the function, sould be between 0 and 1 inclusive.
**/
fixedpt easeOutQuad(fixedpt start, fixedpt end, fixedpt value)
{
	end = end - start;
	end = fixedpt_mul(end, fixedpt_fromint(-1));
	value = fixedpt_mul(value, value - FIXEDPT_TWO);
	return fixedpt_mul(end, value) + start;
}

/**
	Pre-renders the audio data to the sample buffers.
**/
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
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(effect.volume)));
			sampleBuffer++;
			t += dt;
		}
		
		dt = sustainDt;

		start = effect.attackTime;
		end = effect.attackTime + effect.sustainTime;
		for (uint16_t i = start; i < end; ++i)
		{

			fixedpt fpvalue = (*waveFuncPointer)(t);
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(effect.volume)));
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
			*sampleBuffer = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(effect.volume)));
			sampleBuffer++;
			t += dt;
		}
	}
}

/**
	Initalize the audio mixer.
**/
void setupMixer()
{
	fillRandomBuffer();
	renderEffects();
}

/**
	Plays a audio effect.
	@param index	The audio effect to play. Should be a number between 0 and 7, inlusive.
**/
void playEffect(uint8_t index)
{
	isPlaying = true;
	playEffectReadCursor = 0;
	*SCR = 0b0100; // Disable sleep on exit so timer will continue
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


/**
	Plays the melody.
	@return 	Is the melody done playing
**/
bool playNextMelodySample()
{
	playEffect(melodySamples[melodyIndex]);
	melodyIndex++;

	if(melodyIndex >= 7) // Is the melody done?
	{
		playingMelody = false;
		return true;
	} else {
		return false;
	}
}

/**
	Return the next sample of the currently playing sample.
	@return 	The next sample to send to the DAC.
**/
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
			if (!playingMelody)
			{
				*SCR = 0b0110; // Reenable sleep on exit
			}
		}
	}

	return sample;
}
