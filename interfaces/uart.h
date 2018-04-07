#pragma once

#ifdef __cplusplus

#include "dma.h"
#include "mc_hardware_interfaces_uart.h"
#include "stm32f1xx_hal_conf.h"
#include "user_os.h"

struct uart_cfg {
	USART_TypeDef               *UARTx;
	uint32_t					baudrate;
	uint32_t					mode;							// UART_MODE_RX/UART_MODE_TX/UART_MODE_TX_RX.
    DMA_Channel_TypeDef*         dma_tx;                         // Из мерии DMAx_Streamx.
    uint32_t                    dma_tx_ch;                      // Из серии DMA_CHANNEL_x.
	uint8_t						handler_prio;					// 1, 2..15.
};

class uart : public uart_base {
public:
	uart( const uart_cfg* const cfg );
	bool 			reinit		( void ) const;

	void 			on     		( void ) const;
	void 			off    		( void ) const;

    BASE_RESULT		tx			( const uint8_t* const  p_array_tx, const uint16_t& length, const uint32_t& timeout_ms ) const;
    BASE_RESULT		get_byte	( uint8_t* data ) const;

    void			irq_handler				( void ) const;

    void			give_semaphore			( void ) const;

private:
    bool 			init_clk	 			( void ) const;

    const uart_cfg*										const cfg;
    mutable DMA_HandleTypeDef							hdma_tx;
    mutable	UART_HandleTypeDef							handle;
    mutable USER_OS_STATIC_BIN_SEMAPHORE				s = nullptr;
    mutable USER_OS_STATIC_MUTEX                    	m = nullptr;
    mutable USER_OS_STATIC_BIN_SEMAPHORE_BUFFER     	sb;
    mutable USER_OS_STATIC_MUTEX_BUFFER             	mb;
};

#endif
