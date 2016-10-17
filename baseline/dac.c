#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{
	*CMU_HFPERCLKEN0 = *CMU_HFPERCLKEN0 | (1 << 17); // Enable clock signal to DAC
	*DAC0_CTRL = 0x50010; // Prescae DAC clock
	*DAC0_CH0CTRL = 1; // Enable left channel
	*DAC0_CH1CTRL = 1; // Enable right channel
}
