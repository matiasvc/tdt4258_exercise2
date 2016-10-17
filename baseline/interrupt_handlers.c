#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"


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
