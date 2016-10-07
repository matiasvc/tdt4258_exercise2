#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

uint16_t dacValue = 0;
uint16_t countValue = 0;
uint16_t countLength = 10;

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	countValue++;
	if (countValue <= countLength/2)
	{
		dacValue = 0;
	}
	else if (countValue <= countLength)
	{
		dacValue = 0x3FF;
	}
	else
	{
		countValue = 0;
	}

	//*GPIO_PA_DOUT = 0xAAAA;
	//*GPIO_PA_DOUT = dacValue << 8; 
	*DAC0_CH0DATA = dacValue;
	*DAC0_CH1DATA = dacValue;
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
