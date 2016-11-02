#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/**
	Sets up the GIPO input & output.
**/
void setupGPIO()
{
	*CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	// enable GPIO clock

	// Output
	*GPIO_PA_CTRL = 2;	// set high drive strength
	*GPIO_PA_MODEH = 0x55555555; // set pins A8-15 as output
	*GPIO_PA_DOUT = 0xFF00; // Turn off LEDs

	// Input
	*GPIO_PC_MODEL = 0x33333333; // Set pins 0-7 to input
	*GPIO_PC_DOUT = 0xFF; // Set pins to internal pull-up

	// Input interrupts
	*GPIO_EXTIPSELL = 0x22222222; // Enable iterrupts on input pins
	*GPIO_EXTIFALL = 0xFF; // Set interrupt on falling edge
	*GPIO_IEN = 0xFF; // Enable interrupt generation
}

/**
	Checks if a butten has been pressed down since the last check.
	@return 0 if no button is pressed or a number 1-8 for the button that has been pressed.
**/
uint8_t getInput()
{
	uint8_t currentInput = *GPIO_PC_DIN;
	*GPIO_PA_DOUT = currentInput << 8;

	uint8_t buttonPressed = 0;

	for (uint8_t i = 1; i <= 8; ++i)
	{
		uint8_t currentState = (~currentInput) & (1 << (i-1));

		if (currentState)
		{
			buttonPressed = i;
			break;
		}
	}
	return buttonPressed;
}
