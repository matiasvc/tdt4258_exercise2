#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	AudioSample sample = getNextSample();

	// Both samples are stored as int16, meaning they have values on the range -2^15 to 2^15.
	// We add 2^15 to bring it into the value range 0 to 2^16.
	// We then divide by 2^4 to brind the range down to 12 bits, which is what the DAC operates on.

	uint16_t right = (uint16_t)((((int32_t)sample.right) + 0x8000)/16);
	uint16_t left = (uint16_t)((((int32_t)sample.left) + 0x8000)/16);
	
	*DAC0_CH0DATA = right;
	//*DAC0_CH1DATA = left;
	*TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
	/* TODO handle button pressed event, remember to clear pending interrupt */
}
