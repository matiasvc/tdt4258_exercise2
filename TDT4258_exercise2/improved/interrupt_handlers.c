#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"

uint8_t getInput();
bool playNextMelodySample();

void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	int16_t sample = getNextSample();

	// The sample are stored as int16, meaning they have values on the range -2^15 to 2^15.
	// We add 2^15 to bring it into the value range 0 to 2^16.
	// We then divide by 2^4 to bring the range down to 12 bits, which is what the DAC operates on.
	uint16_t scaledSample = (uint16_t)((((int32_t)sample) + 0x8000)/16);
	
	*DAC0_CH0DATA = scaledSample;
	*DAC0_CH1DATA = scaledSample;
	*TIMER1_IFC = 1;
}

void __attribute__ ((interrupt)) TIMER2_IRQHandler()
{
	if (playNextMelodySample())
	{
		*SCR = 0b0110; // Enable deep sleep & sleep on interrupt return
		*TIMER2_IEN = 0;
		*TIMER2_CMD = 0; // Stop the timer
	}

	*TIMER2_IFC = 1;
}

void handleInput()
{
	uint8_t buttonPressed = getInput();

	switch(buttonPressed)
	{
	   case 1: { playEffect(0); } break;
	   case 2: { playEffect(1); } break;
	   case 3: { playEffect(2); } break;
	   case 4: { playEffect(3); } break;
	   case 5: { playEffect(4); } break;
	   case 6: { playEffect(5); } break;
	   case 7: { playEffect(6); } break;
	   case 8: { playEffect(7); } break;
	}
}

void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	*GPIO_IFC = *GPIO_IF;
	handleInput();
}

void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	*GPIO_IFC = *GPIO_IF;
	handleInput();
}
