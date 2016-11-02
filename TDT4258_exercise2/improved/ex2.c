#define   FIXEDPT_WBITS 16 // Use 16 bits of the fixed point type for whole number part
#define   SAMPLE_PERIOD 1750 // Gives 8000 samples per second
#define   MELODY_PERIOD 0xAFFF // 10 times a second

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"


/* Declaration of peripheral setup functions */
void setupGPIO();
void setupTimer(uint16_t timer1Period, uint16_t timer2Period);
void setupDAC();
void setupNVIC();
void setupMixer();

bool melodyPlaying = true;

int main(void)
{
	/* Call the peripheral setup functions */
	setupGPIO();
   setupMixer();
	setupDAC();
	setupTimer(SAMPLE_PERIOD, MELODY_PERIOD);

	/* Enable interrupt handling */
	setupNVIC();
   
   //*SCR = 0b0110; // Enable deep sleep & sleep on interrupt return

   __asm("wfi"); // Enter sleep mode

   while(1) ;

	return 0;
}

void setupNVIC()
{
   // Enable interrups in NVIC
   // GPIO_EVEN | GPIO_ODD | TIMER1 | TIMER2
   *ISER0 = *ISER0 | (1 << 1) | (1 << 11) | (1 << 12) | (1 << 13);
}
