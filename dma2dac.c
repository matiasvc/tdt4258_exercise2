#include "efm32gg.h"

typedef struct
{
  volatile uint32_t src_data_end_ptr;
  volatile uint32_t dst_data_end_ptr;
  volatile uint32_t channel_cfg;
} DMAChannelConfig;

void abc()
{
  // channel 0 config registers at 0x400c2000 + 0x1100

  // Enable the PRS and timer clocks with CMU HFPERCLKEN0
  *CMU_HFPERCLKEN0 |= (1 << 15) | (1 << 7); // PRS(15) and TIMER2 (5) // EFM32GG_RM p150

  // Enable the DMA clock. This is not done with CMU HFPERCLKEN0, but with CMU HFCORECLKEN0. See section 11 [13].
  *CMU_HFCORECLKEN0 |= (1 << 0); // DMA at bit 0

  // Enable DMA as written on page 72
  *DMA_CONFIG |= (1 << 0);

  // Setup the PRS system such that one of the timers trigger a PRS channel
  // Sourcesel = input source to PRS channel, bit position 16 to 21 in PRS_CH0_CTRL. TIMER2 = 0b011110
  // SIGEL = input signal to PRS channel, bit position 0 til 2 in PRS_CH0_CTRL. TIMER2OF (overflow) = 0b001
  *PRS_CH0_CTRL |= (0b0001 << 0) | (0b011110 << 16); // EFM32GG_RM p171

  // Setup the timer with correct period (same as before). No need for timer interrupts now, this is just to create the PRS trigger
  *TIMER2_TOP = 1750;

  // Setup the DAC to trigger on the PRS channel.
  *DAC0_CTRL|= (1 << 6); // EFM32GG_RM p719

  // Setup the DMA control block for Ping Pong mode (section 8.4.3 in [13]). Note that the control block must be 512 byte aligned in memory.
  //*DMA_CHENS |= (1 << 0); // Write high priority to channel 0 // EFM32GG_RM p85
  // TODO Write base address for the primary data structure to DMA_CTRLBASE. For only channel 0 primary data structure, 16 bits are required. see more page 64
  *DMA_CTRLBASE = --;
  DMAChannelConfig *channel0conf = (DMAChannelConfig *)(*DMA_CTRLBASE);
  channel0conf->src_data_end_ptr = --;
  channel0conf->dst_data_end_ptr = --;
  channel0conf->channel_cfg = (0b11 << 30) | (0b01 << 28) | (0b01 << 26) | (0b01 << 24) | (0b001 << 0); // EFM32GG_RM p66


  // Setup the DMA to send a new sample to the DAC every time the DAC is ready (SOURCESEL and SIGSEL in DMA CH0 CTRL).
  // Sourcesel = input source to DMA channel, bit position 16 to 21 in DMA_CH0_CTRL. DAC0 = 0b001010
  // SIGEL = input signal to DMA channel, bit position 0 til 3 in DMA_CH0_CTRL. DAC0CH0 = 0b0000
  *DMA_CH0_CTRL |= (0b0000 << 0) | (0b001010 << 16);  // EFM32GG_RM p96

  // Choose primary structure for channel 0, as described on page 72
  *DMA_CHALTC |= (1 << 0); // Enabling primary structure for this channel

  // Enable DMA interrupts and use the DMA interrupt handler to generate new blocks of data for the DMA to transfer.
  *DMA_CHENS |= (1 << 0); // Enabling DMA channel 0  // EFM32GG_RM p82
}
