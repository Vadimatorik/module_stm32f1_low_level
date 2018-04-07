#include "dma.h"

void dma_clk_on ( void ) {
	__HAL_RCC_DMA1_CLK_ENABLE();
}

void dma_irq_on ( DMA_Channel_TypeDef* dma, uint32_t prio ) {
    switch ( (uint32_t)dma ) {
	case DMA1_Channel1_BASE: NVIC_SetPriority( DMA1_Channel1_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel1_IRQn ); break;
	case DMA1_Channel2_BASE: NVIC_SetPriority( DMA1_Channel2_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel2_IRQn ); break;
	case DMA1_Channel3_BASE: NVIC_SetPriority( DMA1_Channel3_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel3_IRQn ); break;
	case DMA1_Channel4_BASE: NVIC_SetPriority( DMA1_Channel4_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel4_IRQn ); break;
	case DMA1_Channel5_BASE: NVIC_SetPriority( DMA1_Channel5_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel5_IRQn ); break;
	case DMA1_Channel6_BASE: NVIC_SetPriority( DMA1_Channel6_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel6_IRQn ); break;
	case DMA1_Channel7_BASE: NVIC_SetPriority( DMA1_Channel7_IRQn, prio );    NVIC_EnableIRQ( DMA1_Channel7_IRQn ); break;
    };
}
