#include <stdlib.h>
#include <math.h>

#include "efm32gg.h"
#include "audioMixer.h"

// 8000 samples per second
#define SAMPLES_PER_SECOND 8000
#define SAMPLES 4096
#define PI (3.141592653589793)

uint16_t readCursor = 0;
uint16_t writeCursor = 0;

// Audio simples are stored as:
// sample0_right, sample0_left, sample1_right, sample1_left...
static int16_t sampleBuffer[SAMPLES*2];

#define RANDOM_BUFFER_LENGTH 4096
#define RANDOM_MAX 4294967295
static uint32_t randomBuffer[RANDOM_BUFFER_LENGTH];


void renderEffect(AudioEffect effect);



void fillRandomBuffer()
{
	uint32_t random = 879186745;

	for (int i = 0; i < RANDOM_BUFFER_LENGTH; ++i)
	{
		random = (1103515245 * random + 12345) % RANDOM_MAX;
		randomBuffer[i] = random;
	}
}

float squareWave(float t)
{
	float value = sin(t*2*PI)*0.5 + 1;
	return (float)(((int32_t)value)*2 - 1);
}

float sineWave(float t)
{
	return sin(t*2*PI);
}

float sawtoothWave(float t)
{
	return fmod(t, 1.0)*2.0 - 1.0;
}

float noiseWave(float t)
{
	uint32_t index = ((uint32_t)t) % RANDOM_BUFFER_LENGTH;
	return (((float)randomBuffer[index]) / RANDOM_MAX)*2 - 1;
}


float easeInQuad(float start, float end, float value)
{
	end -= start;
	return end * value * value + start;
}

float easeOutQuad(float start, float end, float value)
{
	end -= start;
	return -end * value * (value - 2) + start;
}

float easeInstant(float start, float end, float value)
{
	return end;
}

void setupMixer()
{
	fillRandomBuffer();

	AudioEffect effect = {};
	effect.type = SINE;
	effect.attackTime = 0.1;
	effect.sustainTime = 0.2;
	effect.decayTime = 0.1;
	effect.attackFrequency = 200;
	effect.sustainFrequency = 3000;
	effect.decayFrequency = 100;
	effect.frequencyAttackTransition = EASE_IN;
	effect.frequencyDecayTransition = EASE_OUT;


	renderEffect(effect);
	/*
	float t = 0;
	float dt = 0.01;

	for (int i = 0; i < SAMPLES; i++)
	{
		sampleBuffer[2*i] = (int16_t)(noiseWave(t) * 20760);
		sampleBuffer[2*i+1] = (int16_t)(sawtoothWave(t) * 20760);

		t += dt;
		dt += 0.00001;
	}
	*/
}



void renderEffect(AudioEffect effect)
{
	

	float (*waveFuncPointer)(float);

	float (*volumeAttackFuncPointer)(float, float, float);
	float (*volumeDecayFuncPointer)(float, float, float);

	float (*frequenceAttackFuncPointer)(float, float, float);
	float (*frequenceDecayFuncPointer)(float, float, float);

	switch (effect.type)
	{
		case SINE : { waveFuncPointer = &sineWave; } break;
		case SQUARE : { waveFuncPointer = &squareWave; } break;
		case TRIANGE : { waveFuncPointer = &sineWave; } break;
		case SAWTOOTH : { waveFuncPointer = &sawtoothWave; } break;
		case NOISE : { waveFuncPointer = &noiseWave; } break;
	}

	switch (effect.volumeAttackTransition)
	{
		case INSTANT : { volumeAttackFuncPointer = &easeInstant; } break;
		case EASE_IN : { volumeAttackFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { volumeAttackFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.volumeDecayTransition)
	{
		case INSTANT : { volumeDecayFuncPointer = &easeInstant; } break;
		case EASE_IN : { volumeDecayFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { volumeDecayFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.frequencyAttackTransition)
	{
		case INSTANT : { frequenceAttackFuncPointer = &easeInstant; } break;
		case EASE_IN : { frequenceAttackFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { frequenceAttackFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.frequencyDecayTransition)
	{
		case INSTANT : { frequenceDecayFuncPointer = &easeInstant; } break;
		case EASE_IN : { frequenceDecayFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { frequenceDecayFuncPointer = &easeOutQuad; } break;
	}

	uint16_t nAttackSamples = (uint16_t)(effect.attackTime * SAMPLES_PER_SECOND);
	uint16_t nSustainSamples = (uint16_t)(effect.sustainTime * SAMPLES_PER_SECOND);
	uint16_t nDecaySamples = (uint16_t)(effect.decayTime * SAMPLES_PER_SECOND);

	float attakStartDt = ((float)effect.attackFrequency)/SAMPLES_PER_SECOND;
	float sustainDt = ((float)effect.sustainFrequency)/SAMPLES_PER_SECOND;
	float decayEndDt = ((float)effect.decayFrequency)/SAMPLES_PER_SECOND;

	float t = 0;
	float dt = 0;

	for (int i = 0; i < nAttackSamples; ++i)
	{
		float lerp = ((float)i)/(nAttackSamples-1);
		dt = (*frequenceAttackFuncPointer)(attakStartDt, sustainDt, lerp);

		int16_t value = (int16_t)((*waveFuncPointer)(t) * 20760);
		sampleBuffer[2*i] = value;
		sampleBuffer[2*i + 1] = (int16_t)(dt * 10000);
		t += dt;
	}

	dt = sustainDt;
	uint16_t offset = nAttackSamples;

	for (int i = 0; i < nSustainSamples; ++i)
	{
		int16_t value = (int16_t)((*waveFuncPointer)(t) * 20760);
		sampleBuffer[offset + 2*i] = value;
		sampleBuffer[2*i + 1] = (int16_t)(dt * 10000);
		t += dt;
	}

	offset += nSustainSamples;

	for (int i = 0; i < nDecaySamples; ++i)
	{
		float lerp = ((float)i)/(nAttackSamples-1);
		dt = (*frequenceDecayFuncPointer)(sustainDt, decayEndDt, lerp);

		int16_t value = (int16_t)((*waveFuncPointer)(t) * 20760);
		sampleBuffer[offset + 2*i] = value;
		sampleBuffer[2*i + 1] = (int16_t)(dt * 10000);
		t += dt;
	}
	
	
}

AudioSample getNextSample() {
	AudioSample sample = {};

	sample.right = sampleBuffer[2*readCursor];
	sample.left = sampleBuffer[2*readCursor + 1];

	readCursor++;

	if (readCursor >= SAMPLES) { readCursor = 0; }

	return sample;
}
