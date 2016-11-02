#define   FIXEDPT_WBITS 16 // Use 16 bits of the fixed point type for whole number part
#define   SAMPLE_PERIOD 1750 // Gives 8000 samples per second

#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "audioMixer.h"


/* Declaration of peripheral setup functions */
void setupGPIO();
void setupTimer(uint16_t timer1Period);
void setupDAC();
void setupNVIC();
void setupMixer();


uint8_t getInput();

int main(void)
{
	/* Call the peripheral setup functions */
	setupGPIO();
   setupMixer();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	/* Enable interrupt handling */
	setupNVIC();
   
   playMelody();

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
   // Enable TIMER1 interrups in NVIC
   *ISER0 = *ISER0 | (1 << 12);
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
