#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t timer1Period, uint16_t timer2Period)
{
	*CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | (1 << 6) | (1 << 7); // Enable timer 1 & 2 module clock

	*TIMER1_TOP = timer1Period; // Set timer period
	*TIMER1_IEN = 1; // Enable timer interrupt generation
	*TIMER1_CMD = 1; // Start timer

	*TIMER2_CTRL |= (6 << 24); // Set timer prescaler to a dividing factor of 128
	*TIMER2_TOP = timer2Period; // Set timer period
	*TIMER2_IEN = 1; // Enable timer interrupt generation
	*TIMER2_CMD = 1; // Start timer
}
