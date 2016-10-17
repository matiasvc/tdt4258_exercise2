#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"

#define   FIXEDPT_WBITS 16 // Use 16 bits of the fixed point type for whole number part
#define   SAMPLE_PERIOD 1750 // Gives 8000 samples per second
#define   FRAME_PERIOD  58333 // Gives 30 frames per second

/* Declaration of peripheral setup functions */
void setupGPIO();
void setupTimer(uint16_t timer1Period, uint16_t timer2Period);
void setupDAC();
void setupNVIC();
void setupMixer();

uint8_t getInput();

/* Your code will start executing here */
int main(void)
{
	/* Call the peripheral setup functions */
	setupGPIO();
   setupMixer();
	setupDAC();
	setupTimer(SAMPLE_PERIOD, FRAME_PERIOD);

	/* Enable interrupt handling */
	setupNVIC();


	/* TODO for higher energy efficiency, sleep while waiting for interrupts
	   instead of infinite loop for busy-waiting
	 */
   /*
   AudioEffect effect = {};
   effect.type = NOISE;
   effect.attackTime = 0.2;
   effect.sustainTime = 0.1;
   effect.decayTime = 0.2;
   effect.attackVolume = 
   effect.attackFrequency = 100;
   effect.sustainFrequency = 1000;
   effect.decayFrequency = 100;
   effect.frequencyAttackTransition = EASE_IN;
   effect.frequencyDecayTransition = EASE_OUT;
   */
   while (1)
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

	return 0;
}

void setupNVIC()
{
	/* TODO use the NVIC ISERx registers to enable handling of interrupt(s)
	   remember two things are necessary for interrupt handling:
	   - the peripheral must generate an interrupt signal
	   - the NVIC must be configured to make the CPU handle the signal
	   You will need TIMER1, GPIO odd and GPIO even interrupt handling for this
	   assignment.
	 */
   // Enable timer interrups in NVIC
   // GPIO_EVEN | GPIO_ODD | TIMER1 | TIMER2
   *ISER0 = *ISER0 | (1 << 1) | (1 << 11) | (1 << 12) | (1 << 13);
   //*IPR12 = 0b00000000;
   //*IPR13 = 0b11100000;
}

/* if other interrupt handlers are needed, use the following names: 
   NMI_Handler
   HardFault_Handler
   MemManage_Handler
   BusFault_Handler
   UsageFault_Handler
   Reserved7_Handler
   Reserved8_Handler
   Reserved9_Handler
   Reserved10_Handler
   SVC_Handler
   DebugMon_Handler
   Reserved13_Handler
   PendSV_Handler
   SysTick_Handler
   DMA_IRQHandler
   GPIO_EVEN_IRQHandler
   TIMER0_IRQHandler
   USART0_RX_IRQHandler
   USART0_TX_IRQHandler
   USB_IRQHandler
   ACMP0_IRQHandler
   ADC0_IRQHandler
   DAC0_IRQHandler
   I2C0_IRQHandler
   I2C1_IRQHandler
   GPIO_ODD_IRQHandler
   TIMER1_IRQHandler
   TIMER2_IRQHandler
   TIMER3_IRQHandler
   USART1_RX_IRQHandler
   USART1_TX_IRQHandler
   LESENSE_IRQHandler
   USART2_RX_IRQHandler
   USART2_TX_IRQHandler
   UART0_RX_IRQHandler
   UART0_TX_IRQHandler
   UART1_RX_IRQHandler
   UART1_TX_IRQHandler
   LEUART0_IRQHandler
   LEUART1_IRQHandler
   LETIMER0_IRQHandler
   PCNT0_IRQHandler
   PCNT1_IRQHandler
   PCNT2_IRQHandler
   RTC_IRQHandler
   BURTC_IRQHandler
   CMU_IRQHandler
   VCMP_IRQHandler
   LCD_IRQHandler
   MSC_IRQHandler
   AES_IRQHandler
   EBI_IRQHandler
   EMU_IRQHandler
*/
