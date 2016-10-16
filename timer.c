#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t timer1Period, uint16_t timer2Period)
{
	/*
	   TODO enable and set up the timer

	   1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0
	   2. Write the period to register TIMER1_TOP
	   3. Enable timer interrupt generation by writing 1 to TIMER1_IEN
	   4. Start the timer by writing 1 to TIMER1_CMD

	   This will cause a timer interrupt to be generated every (period) cycles. Remember to configure the NVIC as well, otherwise the interrupt handler will not be invoked.
	 */
	*CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | (1 << 6) | (1 << 7); // Enable timer 1 & 2 module clock

	*TIMER1_TOP = timer1Period; // Set timer period
	*TIMER1_IEN = 1; // Enable timer interrupt generation
	*TIMER1_CMD = 1; // Start timer

	*TIMER2_CTRL = *TIMER2_CTRL | (3 << 24); // Prescale timer by dividing HFPERCLK by 8
	*TIMER2_TOP = timer2Period;
	*TIMER2_IEN = 1;
	*TIMER2_CMD = 1;
}
