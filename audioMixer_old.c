#include <stdlib.h>
#include <math.h>

#include "efm32gg.h"
#include "audioMixer.h"

#include "fixedptc.h"

#define SAMPLES_PER_SECOND 16000
#define SAMPLES 16000
//#define WRITE_AHEAD 1600 // Write samples ahead 1/10 of a second
//#define WRITE_AHEAD 1600

//static uint32_t readCursor = 0; // The next sample to be rea dto the DAC
//static uint32_t writeCursor = 0; // The next sample to be rendered


// Audio simples are stored as:
// sample0_right, sample0_left, sample1_right, sample1_left, ...
//static int16_t sampleBuffer[SAMPLES*2];

#define RANDOM_BUFFER_LENGTH 4096
#define RANDOM_MAX 4294967295
static uint32_t randomBuffer[RANDOM_BUFFER_LENGTH];

//
static uint8_t sample0Buffer[16000];
static uint8_t sample1Buffer[16000];
static uint8_t sample2Buffer[16000];
static uint8_t sample3Buffer[16000];
//static uint8_t sample4Buffer[16000];
//static uint8_t sample5Buffer[16000];
//static uint8_t sample6Buffer[16000];
//static uint8_t sample7Buffer[16000];

/*
typedef struct
{
	AudioEffect effect;

	uint16_t nAttackSamples;
	uint16_t nSustainSamples;
	uint16_t nDecaySamples;

	uint32_t startSampleTime;
	uint32_t endSampleTime;

	uint32_t sustainStartSampleTime;
	uint32_t decayStartSampleTime;

	fixedpt (*waveFuncPointer)(fixedpt);

	fixedpt (*volumeAttackFuncPointer)(fixedpt, fixedpt, fixedpt);
	fixedpt (*volumeDecayFuncPointer)(fixedpt, fixedpt, fixedpt);

	fixedpt (*frequenceAttackFuncPointer)(fixedpt, fixedpt, fixedpt);
	fixedpt (*frequenceDecayFuncPointer)(fixedpt, fixedpt, fixedpt);

	fixedpt attakStartDt;
	fixedpt sustainDt;
	fixedpt decayEndDt;

	fixedpt t;
	fixedpt dt;

} ChannelSample ;

#define NUMBER_OF_EFFECTS 8
static uint8_t activeChannels;
static ChannelSample channels[NUMBER_OF_EFFECTS];
*/
//void renderEffect(AudioEffect effect);


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

void setupMixer()
{
	fillRandomBuffer();
	
	
	
	//renderAudio();
	//renderEffect(effect);
	/*

	/*
	fixedpt t = 0;
	fixedpt dt = fixedpt_rconst(0.008);

	for (uint16_t i = 0; i < SAMPLES; ++i)
	{
		fixedpt fpvalue = sineWave(t);
		sampleBuffer[2*i] = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(5000)));
		//fpvalue = squareWave(t);
		//sampleBuffer[2*i + 1] = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(5000)));
		//float lerp = ((float)i)/(nAttackSamples-1);
		fixedpt lerp = fixedpt_div(fixedpt_fromint(i), fixedpt_fromint(SAMPLES) - FIXEDPT_ONE);
		sampleBuffer[2*i+1] = (int16_t)fixedpt_toint(fixedpt_mul(lerp, fixedpt_fromint(5000)));
		t += dt;
	}
	*/
}

/*
void stopProgram()
{
	int a = 2;
	a++;
}

void wrong()
{
	int a = 2;
	a++;	
}

void playEffect(AudioEffect effect)
{
	uint8_t index = 0xFF;

	for (uint8_t channelIndex = 0; channelIndex < NUMBER_OF_EFFECTS; channelIndex++)
	{
		if ((activeChannels & (1 << channelIndex)) == 0)
		{
			index = channelIndex;
			break;
		}
	}

	if (index == 0xFF)
	{
		return; // No free sample spot, so we ignore the request.
	}

	ChannelSample channelSample = {};

	channelSample.nAttackSamples = (uint16_t)(effect.attackTime * SAMPLES_PER_SECOND);
	channelSample.nSustainSamples = (uint16_t)(effect.sustainTime * SAMPLES_PER_SECOND);
	channelSample.nDecaySamples = (uint16_t)(effect.decayTime * SAMPLES_PER_SECOND);

	channelSample.startSampleTime = writeCursor;
	channelSample.endSampleTime = channelSample.endSampleTime + channelSample.nAttackSamples + channelSample.nSustainSamples + channelSample.nDecaySamples;

	channelSample.sustainStartSampleTime = channelSample.startSampleTime + channelSample.nAttackSamples;
	channelSample.decayStartSampleTime = channelSample.sustainStartSampleTime + channelSample.nSustainSamples;

	switch (effect.type)
	{
		case SINE : { channelSample.waveFuncPointer = &sineWave; } break;
		case SQUARE : { channelSample.waveFuncPointer = &squareWave; } break;
		case SAWTOOTH : { channelSample.waveFuncPointer = &sawtoothWave; } break;
		case NOISE : { channelSample.waveFuncPointer = &noiseWave; } break;
	}

	switch (effect.volumeAttackTransition)
	{
		case EASE_IN : { channelSample.volumeAttackFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { channelSample.volumeAttackFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.volumeDecayTransition)
	{
		case EASE_IN : { channelSample.volumeDecayFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { channelSample.volumeDecayFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.frequencyAttackTransition)
	{
		case EASE_IN : { channelSample.frequenceAttackFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { channelSample.frequenceAttackFuncPointer = &easeOutQuad; } break;
	}

	switch (effect.frequencyDecayTransition)
	{
		case EASE_IN : { channelSample.frequenceDecayFuncPointer = &easeInQuad; } break;
		case EASE_OUT : { channelSample.frequenceDecayFuncPointer = &easeOutQuad; } break;
	}

	channelSample.attakStartDt = fixedpt_div(fixedpt_fromint(effect.attackFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));
	channelSample.sustainDt = fixedpt_div(fixedpt_fromint(effect.sustainFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));
	channelSample.decayEndDt = fixedpt_div(fixedpt_fromint(effect.decayFrequency), fixedpt_fromint(SAMPLES_PER_SECOND));

	channelSample.t = 0;

	channels[index] = channelSample;
	activeChannels |= (1 << index); // Set the channel as active
}

void renderAudio()
{	

	for (uint32_t sampleIndex = writeCursor; sampleIndex < readCursor + WRITE_AHEAD; sampleIndex++)
	{
		sampleBuffer[(2*writeCursor + 1) % (2*SAMPLES)] = (int16_t)writeCursor - (int16_t)readCursor;
		int16_t rightSample = 0;
		int16_t leftSample = 0;

		for (uint8_t index = 0; index < NUMBER_OF_EFFECTS; index++)
		{
			if ((activeChannels & (1 << index)) == 0) { continue; } // Channel is not active, so we skip it

			fixedpt fpvalue = (*channels[index].waveFuncPointer)(channels[index].t);
			rightSample += (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20000)));

			if ((sampleIndex >= channels[index].startSampleTime) && (sampleIndex < channels[index].sustainStartSampleTime))
			{
				// Attack
				fixedpt lerp = fixedpt_div(fixedpt_fromint(sampleIndex - channels[index].startSampleTime), fixedpt_fromint(channels[index].nAttackSamples));
				fixedpt dt = (*channels[index].frequenceAttackFuncPointer)(channels[index].attakStartDt, channels[index].sustainDt, lerp);
				channels[index].dt = dt;
				channels[index].t += dt;

				//sampleBuffer[(2*writeCursor + 1) % (2*SAMPLES)] = -500;
				
			}
			else if ((sampleIndex >= channels[index].sustainStartSampleTime) && (sampleIndex < channels[index].decayStartSampleTime))
			{
				// Sustain
				channels[index].t += channels[index].dt;
				//sampleBuffer[(2*writeCursor + 1) % (2*SAMPLES)] = -200;
			}
			else if ((sampleIndex >= channels[index].decayStartSampleTime) && (sampleIndex < channels[index].endSampleTime))
			{
				// Decay
				fixedpt lerp = fixedpt_div(fixedpt_fromint(sampleIndex - channels[index].decayStartSampleTime), fixedpt_fromint(channels[index].nDecaySamples));
				fixedpt dt = (*channels[index].frequenceAttackFuncPointer)(channels[index].sustainDt, channels[index].decayEndDt, lerp);
				channels[index].t += dt;

				//sampleBuffer[(2*writeCursor + 1) % (2*SAMPLES)] = 300;
			}
			else if (sampleIndex >= channels[index].endSampleTime)
			{
				// Sample has been completely rendered
				activeChannels &= ~(1 << index); // Free up the channel
				sampleBuffer[(2*writeCursor + 1) % (2*SAMPLES)] = 500;
				//stopProgram();
			}

		}

		sampleBuffer[(2*writeCursor) % (2*SAMPLES)] = rightSample;
		//sampleBuffer[2*(writeCursor % SAMPLES) + 1] = leftSample;

		writeCursor++;
	}
}
*/

void renderEffect(AudioEffect effects[], uint8_t sampleNum)
{	
	for (uint8_t effectIndex = 0; effectIndex < 8; effectIndex++)
	{		
		uint16_t nAttackSamples = (uint16_t)(effect.attackTime * SAMPLES_PER_SECOND);
		uint16_t nSustainSamples = (uint16_t)(effect.sustainTime * SAMPLES_PER_SECOND);
		uint16_t nDecaySamples = (uint16_t)(effect.decayTime * SAMPLES_PER_SECOND);
		uint16_t nTotalSamples = nAttackSamples + nSustainSamples + nDecaySamples;

		

		switch(effectIndex) // This would have been a lot prettier if malloc worked :(
		{
			case 0:
			{
				sample0Buffer =
			} break;
			case 1:
			{

			} break;
			case 2:
			{

			} break;
			case 3:
			{

			} break;
			case 4:
			{

			} break;
			case 5:
			{

			} break;
			case 6:
			{

			} break;
			case 7:
			{

			} break;
		}

		fixedpt (*waveFuncPointer)(fixedpt);

		fixedpt (*volumeAttackFuncPointer)(fixedpt, fixedpt, fixedpt);
		fixedpt (*volumeDecayFuncPointer)(fixedpt, fixedpt, fixedpt);

		fixedpt (*frequenceAttackFuncPointer)(fixedpt, fixedpt, fixedpt);
		fixedpt (*frequenceDecayFuncPointer)(fixedpt, fixedpt, fixedpt);

		switch (effect.type)
		{
			case SINE : { waveFuncPointer = &sineWave; } break;
			case SQUARE : { waveFuncPointer = &squareWave; } break;
			case SAWTOOTH : { waveFuncPointer = &sawtoothWave; } break;
			case NOISE : { waveFuncPointer = &noiseWave; } break;
		}

		switch (effect.volumeAttackTransition)
		{
			case EASE_IN : { volumeAttackFuncPointer = &easeInQuad; } break;
			case EASE_OUT : { volumeAttackFuncPointer = &easeOutQuad; } break;
		}

		switch (effect.volumeDecayTransition)
		{
			case EASE_IN : { volumeDecayFuncPointer = &easeInQuad; } break;
			case EASE_OUT : { volumeDecayFuncPointer = &easeOutQuad; } break;
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
		uint16_t end = nAttackSamples;
		for (uint16_t i = start; i < end; ++i)
		{
			fixedpt lerp = fixedpt_div(fixedpt_fromint(i), fixedpt_fromint(nAttackSamples) - FIXEDPT_ONE);
			dt = (*frequenceAttackFuncPointer)(attakStartDt, sustainDt, lerp);

			fixedpt fpvalue = (*waveFuncPointer)(t);
			sampleBuffer[2*i] = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));
			sampleBuffer[2*i + 1] = (int16_t)(fixedpt_toint(fixedpt_mul(dt, fixedpt_fromint(1000))));
			t += dt;
		}
		
		dt = sustainDt;

		start = nAttackSamples;
		end = nAttackSamples + nSustainSamples;
		for (uint16_t i = start; i < end; ++i)
		{

			fixedpt fpvalue = (*waveFuncPointer)(t);
			sampleBuffer[2*i] = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));;
			sampleBuffer[2*i + 1] = (int16_t)(fixedpt_toint(fixedpt_mul(dt, fixedpt_fromint(1000))));
			t += dt;
		}

		start = nAttackSamples + nSustainSamples;
		end = nAttackSamples + nSustainSamples + nDecaySamples;
		for (uint16_t i = start; i < end; ++i)
		{
			fixedpt lerp = fixedpt_div(fixedpt_fromint(i) - fixedpt_fromint(start), fixedpt_fromint(end - start));
			dt = (*frequenceDecayFuncPointer)(sustainDt, decayEndDt, lerp);

			fixedpt fpvalue = (*waveFuncPointer)(t);
			sampleBuffer[2*i] = (int16_t)fixedpt_toint(fixedpt_mul(fpvalue, fixedpt_fromint(20760)));;
			sampleBuffer[2*i + 1] = (int16_t)(fixedpt_toint(fixedpt_mul(dt, fixedpt_fromint(1000))));
			t += dt;
		}
	}
}

inline AudioSample getNextSample() {
	AudioSample sample = {};

	//sample.right = sampleBuffer[(2*readCursor) % (2*SAMPLES)];
	//sample.right = sampleBuffer[(2*readCursor)];
	//sample.left = sampleBuffer[2*(readCursor % SAMPLES) + 1];
	//readCursor++;

	//if (readCursor >= SAMPLES) { stopProgram(); }

	return sample;
}
