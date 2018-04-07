#include "spi.h"
#include <string.h>

spi_master_8bit::spi_master_8bit ( const spi_master_8bit_cfg* const cfg ) : cfg( cfg ) {
    this->handle.Instance                               = cfg->SPIx;
    this->handle.Init.Mode                              = SPI_MODE_MASTER;
    this->handle.Init.Direction                         = SPI_DIRECTION_2LINES;
    this->handle.Init.DataSize                          = SPI_DATASIZE_8BIT;
    this->handle.Init.CLKPolarity                       = cfg->clk_polarity;
    this->handle.Init.CLKPhase                          = cfg->clk_phase;
    this->handle.Init.BaudRatePrescaler                 = cfg->baud_rate_prescaler;

    this->handle.Init.NSS                               = SPI_NSS_SOFT;
    this->handle.Init.FirstBit                          = SPI_FIRSTBIT_MSB;
    this->handle.Init.CRCCalculation                    = SPI_CRCCALCULATION_DISABLE;
    this->handle.Init.CRCPolynomial                     = 0xFF;

    this->handle.obj                                    = this;

    if ( cfg->dma_tx != nullptr ) {
        this->handle.hdmatx = &this->hdma_tx;
        this->handle.hdmatx->Instance					= this->cfg->dma_tx;
        this->handle.hdmatx->Init.Direction				= DMA_MEMORY_TO_PERIPH;
        this->handle.hdmatx->Init.PeriphInc				= DMA_PINC_DISABLE;
        this->handle.hdmatx->Init.MemInc				= DMA_MINC_ENABLE;
        this->handle.hdmatx->Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;
        this->handle.hdmatx->Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;
        this->handle.hdmatx->Init.Mode					= DMA_NORMAL;
        this->handle.hdmatx->Init.Priority				= DMA_PRIORITY_HIGH;

        this->handle.hdmatx                             = &this->hdma_tx;
        this->handle.hdmatx->Parent                     = &this->handle;
    }

    if ( cfg->dma_rx != nullptr ) {
        this->handle.hdmarx = &this->hdma_rx;
        this->handle.hdmarx->Instance					= this->cfg->dma_rx;
        this->handle.hdmarx->Init.Direction				= DMA_PERIPH_TO_MEMORY;
        this->handle.hdmarx->Init.PeriphInc				= DMA_PINC_DISABLE;
        this->handle.hdmarx->Init.MemInc				= DMA_MINC_ENABLE;
        this->handle.hdmarx->Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;
        this->handle.hdmarx->Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;
        this->handle.hdmarx->Init.Mode					= DMA_NORMAL;
        this->handle.hdmarx->Init.Priority				= DMA_PRIORITY_HIGH;

        this->handle.hdmarx                             = &this->hdma_rx;
        this->handle.hdmarx->Parent                     = &this->handle;
    }

    this->m = USER_OS_STATIC_MUTEX_CREATE( &mb );
    this->s = USER_OS_STATIC_BIN_SEMAPHORE_CREATE( &this->sb );
}

BASE_RESULT spi_master_8bit::reinit ( void ) const {
    if ( this->init_clk_spi() == false )    return BASE_RESULT::ERROR_INIT;      // Включаем тактирование SPI.
    //this->init_spi_irq();
    // Включаем IRQ SPI (если DMA не вызывается для используемого функционала).
    if ( this->init_spi() == false )        return BASE_RESULT::ERROR_INIT;
    return BASE_RESULT::OK;
}

void spi_master_8bit::on ( void ) const {
    __HAL_SPI_ENABLE(&this->handle);
}

void spi_master_8bit::off  ( void ) const {
    __HAL_SPI_DISABLE(&this->handle);
}

BASE_RESULT spi_master_8bit::tx ( const uint8_t* const  p_array_tx, const uint16_t& length, const uint32_t& timeout_ms, const SPI::STEP_MODE step_mode  ) const {
    (void)step_mode;

    if ( this->m != nullptr)			USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

    BASE_RESULT rv = BASE_RESULT::TIME_OUT ;
    xSemaphoreTake ( this->s, 0 );

    if ( this->cfg->pin_cs != nullptr )		this->cfg->pin_cs->set( 0 );

    if ( this->handle.hdmatx != nullptr ) {
        HAL_SPI_Transmit_DMA( &this->handle, (uint8_t*)p_array_tx,length);
    }

    if ( xSemaphoreTake ( this->s, timeout_ms ) == pdTRUE ) {
    	rv = BASE_RESULT::OK;
    }

    if ( this->cfg->pin_cs != nullptr)		this->cfg->pin_cs->set( 1 );
    if ( this->m != nullptr)				USER_OS_GIVE_MUTEX( this->m );
    return rv;
}
/*******************************************************************************************************
 * Обработчики.
 *******************************************************************************************************/
void spi_master_8bit::handler ( void ) {
    if ( this->handle.hdmatx != nullptr )
        HAL_DMA_IRQHandler( &this->hdma_tx );

    if ( this->handle.hdmarx != nullptr )
        HAL_DMA_IRQHandler( &this->hdma_rx );
}

void spi_master_8bit::give_semaphore ( void ) {
    if ( this->s ) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR ( this->s, &xHigherPriorityTaskWoken);
    }
}

extern "C" {

void HAL_SPI_TxCpltCallback ( SPI_HandleTypeDef *hspi ) {
     spi_master_8bit* o = ( spi_master_8bit* )hspi->obj;
     o->give_semaphore();
}

void HAL_SPI_RxCpltCallback ( SPI_HandleTypeDef *hspi ) {
     spi_master_8bit* o = ( spi_master_8bit* )hspi->obj;
     o->give_semaphore();
}

void HAL_SPI_TxRxCpltCallback ( SPI_HandleTypeDef *hspi ) {
     spi_master_8bit* o = ( spi_master_8bit* )hspi->obj;
     o->give_semaphore();
}

}

BASE_RESULT spi_master_8bit::tx ( const uint8_t* const  p_array_tx, uint8_t* p_array_rx, const uint16_t& length, const uint32_t& timeout_ms ) const {
    if ( this->m != nullptr)			USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

    BASE_RESULT rv = BASE_RESULT::TIME_OUT;
    xSemaphoreTake ( this->s, 0 );

    if ( this->cfg->pin_cs != nullptr )	this->cfg->pin_cs->set( 0 );

    if ( ( this->handle.hdmatx != nullptr ) && ( this->handle.hdmarx != nullptr ) ) {
        HAL_SPI_TransmitReceive_DMA( &this->handle, (uint8_t*)p_array_tx, p_array_rx, length );
    }

    if ( xSemaphoreTake ( this->s, timeout_ms ) == pdTRUE ) {
            rv = BASE_RESULT::OK;
    }

    if ( this->cfg->pin_cs != nullptr)	this->cfg->pin_cs->set( 1 );
    if ( this->m != nullptr)			USER_OS_GIVE_MUTEX( this->m );
    return rv;
}

BASE_RESULT spi_master_8bit::tx_one_item ( const uint8_t p_item_tx, const uint16_t count, const uint32_t timeout_ms ) const {
	if ( this->m != nullptr)			USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

    BASE_RESULT rv = BASE_RESULT::TIME_OUT ;
    xSemaphoreTake ( this->s, 0 );

    if ( this->cfg->pin_cs != nullptr )	this->cfg->pin_cs->set( 0 );

    uint8_t p_array_tx[count];
    memset(p_array_tx, p_item_tx, count);

    if ( this->handle.hdmatx != nullptr ) {
        HAL_SPI_Transmit_DMA( &this->handle,p_array_tx,count);
    }

    if ( xSemaphoreTake ( this->s, timeout_ms ) == pdTRUE ) {
            rv = BASE_RESULT::OK;
    }

    if ( this->cfg->pin_cs != nullptr)	this->cfg->pin_cs->set( 1 );
    if ( this->m != nullptr)			USER_OS_GIVE_MUTEX( this->m );
    return rv;
}

BASE_RESULT spi_master_8bit::rx ( uint8_t* p_array_rx, const uint16_t& length, const uint32_t& timeout_ms, const uint8_t& out_value ) const {
	if ( this->m != nullptr)		USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );

    BASE_RESULT rv = BASE_RESULT::TIME_OUT ;
    xSemaphoreTake ( this->s, 0 );

    if ( this->cfg->pin_cs != nullptr )     // Опускаем CS (для того, чтобы "выбрать" устроство).
        this->cfg->pin_cs->set( 0 );

    uint8_t tx_dummy[length];
    memset( tx_dummy, out_value, length );

    if ( this->handle.hdmarx != nullptr ) {    // Если указан DMA на прием (чтобы писать поверх уже отправленных данных) - настраиваем DMA в такой режим.
        HAL_SPI_TransmitReceive_DMA( &this->handle, tx_dummy, p_array_rx, length );
    }

    if ( xSemaphoreTake ( this->s, timeout_ms ) == pdTRUE ) {
            rv = BASE_RESULT::OK;
    }

    if ( this->cfg->pin_cs != nullptr)	this->cfg->pin_cs->set( 1 );
    if ( this->m != nullptr)			USER_OS_GIVE_MUTEX( this->m );
    return rv;
}


BASE_RESULT spi_master_8bit::set_prescaler ( uint32_t prescaler ) const {
	if ( this->m != nullptr)			USER_OS_TAKE_MUTEX( this->m, portMAX_DELAY );
    this->handle.Instance->CR1 &= ~( ( uint32_t )SPI_CR1_BR_Msk );
    this->handle.Instance->CR1 |= prescaler;
    if ( this->m != nullptr)			USER_OS_GIVE_MUTEX( this->m );

    return BASE_RESULT::OK;
}

/*******************************************************************************************************
 * Прикладные функции.
 *******************************************************************************************************/
// Включаем тактирование SPI.

extern "C" {
bool spi_master_8bit::init_clk_spi () const {
    switch ((uint32_t)this->cfg->SPIx) {
#ifdef SPI
        case    SPI_BASE:     __SPI_CLK_ENABLE();     break;
#endif
#ifdef SPI1
        case    SPI1_BASE:    __HAL_RCC_SPI1_CLK_ENABLE();    break;
#endif
#ifdef SPI2
        case    SPI2_BASE:    __HAL_RCC_SPI2_CLK_ENABLE();    break;
#endif
#ifdef SPI3
        case    SPI3_BASE:    __HAL_RCC_SPI3_CLK_ENABLE();    break;
#endif
#ifdef SPI4
        case    SPI4_BASE:    __HAL_RCC_SPI4_CLK_ENABLE();    break;
#endif
#ifdef SPI5
        case    SPI5_BASE:    __HAL_RCC_SPI5_CLK_ENABLE();    break;
#endif
#ifdef SPI6
        case    SPI6_BASE:    __HAL_RCC_SPI6_CLK_ENABLE();    break;
#endif
    };
    return true;
}

bool spi_master_8bit::init_spi_irq ( void ) const {
    // Если и TX и RX по DMA, то SPI прерывание не включается.
    if ((this->cfg->dma_tx != nullptr) && (this->cfg->dma_rx != nullptr)) {
        return false;
    }
    switch ((uint32_t)this->cfg->SPIx)    {
#ifdef SPI
        case    SPI_BASE:    NVIC_SetPriority(SPI_IRQn, 6);    NVIC_EnableIRQ(SPI_IRQn);    break;
#endif
#ifdef SPI1
        case    SPI1_BASE:    NVIC_SetPriority(SPI1_IRQn, 6);    NVIC_EnableIRQ(SPI1_IRQn);    break;
#endif
#ifdef SPI2
        case    SPI2_BASE:    NVIC_SetPriority(SPI2_IRQn, 6);    NVIC_EnableIRQ(SPI2_IRQn);    break;
#endif
#ifdef SPI3
        case    SPI3_BASE:    NVIC_SetPriority(SPI3_IRQn, 6);    NVIC_EnableIRQ(SPI3_IRQn);    break;
#endif
#ifdef SPI4
        case    SPI4_BASE:    NVIC_SetPriority(SPI4_IRQn, 6);    NVIC_EnableIRQ(SPI4_IRQn);    break;
#endif
#ifdef SPI5
        case    SPI5_BASE:    NVIC_SetPriority(SPI5_IRQn, 6);    NVIC_EnableIRQ(SPI5_IRQn);    break;
#endif
#ifdef SPI6
        case    SPI6_BASE:    NVIC_SetPriority(SPI6_IRQn, 6);    NVIC_EnableIRQ(SPI6_IRQn);    break;
#endif
};
    return true;
}

}

bool spi_master_8bit::init_spi ( void ) const {
	HAL_StatusTypeDef r;

	HAL_SPI_DeInit( &this->handle );
	r = HAL_SPI_Init ( &this->handle );
	if ( r != HAL_OK ) return false;

    if ( this->cfg->dma_tx != nullptr ) {
        dma_clk_on();
		r = HAL_DMA_Init( &this->hdma_tx );
		if ( r != HAL_OK ) return false;
		dma_irq_on( this->cfg->dma_tx, this->cfg->handler_prio );
    }

    if ( this->cfg->dma_rx != nullptr ) {
        dma_clk_on();
		r = HAL_DMA_Init( &this->hdma_rx );
		if ( r != HAL_OK ) return false;
		dma_irq_on( this->cfg->dma_rx, this->cfg->handler_prio );
    }

    if ( this->cfg->pin_cs != nullptr )		this->cfg->pin_cs->set( 1 );
    return true;
}
