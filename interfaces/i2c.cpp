#include "i2c.h"

i2c::i2c ( const i2c_cfg* const cfg ) : cfg( cfg ) {
    this->s = USER_OS_STATIC_BIN_SEMAPHORE_CREATE( &sb );

    this->handle.obj                     = this;

    this->handle.Instance                = this->cfg->i2c_inst;
    this->handle.Init.ClockSpeed         = cfg->speed;
    this->handle.Init.DutyCycle          = I2C_DUTYCYCLE_2;
    this->handle.Init.OwnAddress1        = 0x18;                         // Адрес нашего контроллера, взят из головы.
    this->handle.Init.AddressingMode     = I2C_ADDRESSINGMODE_7BIT;
    this->handle.Init.GeneralCallMode    = I2C_GENERALCALL_DISABLE;
    this->handle.Init.NoStretchMode      = I2C_NOSTRETCH_DISABLE;

    if ( this->cfg->dma_tx != nullptr ) {
        this->handle.hdmatx                     = &this->dma_tx;

        this->dma_tx.Instance                   = this->cfg->dma_tx;

        this->dma_tx.Init.Direction             = DMA_MEMORY_TO_PERIPH;
        this->dma_tx.Init.PeriphInc             = DMA_PINC_DISABLE;
        this->dma_tx.Init.MemInc                = DMA_MINC_ENABLE;
        this->dma_tx.Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        this->dma_tx.Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        this->dma_tx.Init.Mode                  = DMA_NORMAL;
        this->dma_tx.Init.Priority              = DMA_PRIORITY_HIGH;

        this->dma_tx.Parent                     = &this->handle;
    }

    if ( this->cfg->dma_rx != nullptr ) {
        this->handle.hdmarx                     = &this->dma_rx;

        this->dma_rx.Instance                   = this->cfg->dma_rx;

        this->dma_rx.Init.Direction             = DMA_PERIPH_TO_MEMORY;
        this->dma_rx.Init.PeriphInc             = DMA_PINC_DISABLE;
        this->dma_rx.Init.MemInc                = DMA_MINC_ENABLE;
        this->dma_rx.Init.PeriphDataAlignment   = DMA_PDATAALIGN_BYTE;
        this->dma_rx.Init.MemDataAlignment      = DMA_MDATAALIGN_BYTE;
        this->dma_rx.Init.Mode                  = DMA_NORMAL;
        this->dma_rx.Init.Priority              = DMA_PRIORITY_HIGH;

        this->dma_rx.Parent                     = &this->handle;
    }
}

void i2c::reinit ( void ) const {
    this->init_clk_i2c();

    HAL_I2C_Init( &this->handle );

    this->init_i2c_irq();

    this->dma_clk_on( this->cfg->dma_tx );
    this->dma_clk_on( this->cfg->dma_rx );

    if ( this->cfg->dma_tx != nullptr )
        HAL_DMA_Init( this->handle.hdmatx );

    if ( this->cfg->dma_rx != nullptr )
        HAL_DMA_Init( this->handle.hdmarx );

    this->dma_irq_on( this->cfg->dma_tx );
    this->dma_irq_on( this->cfg->dma_rx );
}

int i2c::read ( uint8_t slave_addr, uint8_t* p_buf, uint8_t read_addr, uint16_t num_byte_to_readr ) const {
    USER_OS_TAKE_BIN_SEMAPHORE( this->s, 0 );
    HAL_I2C_Master_Transmit_IT(&this->handle, slave_addr, &read_addr, 1 );
    if(USER_OS_TAKE_BIN_SEMAPHORE( this->s, 10 ) != pdTRUE){
        return 1;
    }
    HAL_I2C_Master_Receive_IT(&this->handle, slave_addr, p_buf, num_byte_to_readr );
    if(USER_OS_TAKE_BIN_SEMAPHORE( this->s, 10 ) != pdTRUE){
        return 1;
    }
    return 0;
}

int i2c::read_dma ( uint8_t slave_addr, uint8_t* p_buf, uint8_t read_addr, uint16_t num_byte_to_readr ) const {
    USER_OS_TAKE_BIN_SEMAPHORE( this->s, 0);
    HAL_I2C_Master_Transmit_DMA(&this->handle, slave_addr, &read_addr, 1 );
    if(USER_OS_TAKE_BIN_SEMAPHORE( this->s, 10 ) != pdTRUE){
        return 1;
    }
    HAL_I2C_Master_Receive_DMA(&this->handle, slave_addr, p_buf, num_byte_to_readr );
    if(USER_OS_TAKE_BIN_SEMAPHORE( this->s, 100 ) != pdTRUE){
        return 1;
    }
    return 0;
}

int i2c::write_byte ( uint8_t slave_addr, uint8_t* p_buf, uint8_t write_addr ) const {
    uint8_t tx_buf[2] = {0};
    tx_buf[0] = write_addr;
    tx_buf[1] = *p_buf;
    USER_OS_TAKE_BIN_SEMAPHORE( this->s, 0 );
    HAL_I2C_Master_Transmit_IT(&this->handle, slave_addr, tx_buf, 1 );
    if( USER_OS_TAKE_BIN_SEMAPHORE( this->s, 10 ) != pdTRUE ) {
        return 1;
    }
    return 0;
}

void i2c::s_give ( void ) const {
    BaseType_t xHigherPriorityTaskWoken = 0;
    USER_OS_GIVE_BIN_SEMAPHORE_FROM_ISR (this->s, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
}

void i2c::init_clk_i2c ( void ) const {
    switch ( ( uint32_t )this->cfg->i2c_inst ) {
#ifdef I2C
    case    I2C_BASE:     __HAL_RCC_I2C_CLK_ENABLE();      break;
#endif
#ifdef I2C1
    case    I2C1_BASE:    __HAL_RCC_I2C1_CLK_ENABLE();     break;
#endif
#ifdef I2C2
    case    I2C2_BASE:    __HAL_RCC_I2C2_CLK_ENABLE();     break;
#endif
    };
    return;
}

void i2c::init_i2c_irq ( void ) const {
    switch ( ( uint32_t )this->cfg->i2c_inst )    {
#ifdef I2C
    case    I2C_BASE:     NVIC_SetPriority(I2C_EV_IRQn, 6);    NVIC_EnableIRQ(I2C_EV_IRQn);    break;
#endif
#ifdef I2C1
    case    I2C1_BASE:    NVIC_SetPriority(I2C1_EV_IRQn, 6);    NVIC_EnableIRQ(I2C1_EV_IRQn);    break;
#endif
#ifdef I2C2
    case    I2C2_BASE:    NVIC_SetPriority(I2C2_EV_IRQn, 6);    NVIC_EnableIRQ(I2C2_EV_IRQn);    break;
#endif
};
    return;
}

void i2c::dma_clk_on ( DMA_Channel_TypeDef* dma) const {
#ifdef DMA1
    switch ( (uint32_t)dma ) {
    case DMA1_Channel1_BASE:
    case DMA1_Channel2_BASE:
    case DMA1_Channel3_BASE:
    case DMA1_Channel4_BASE:
    case DMA1_Channel5_BASE:
    case DMA1_Channel6_BASE:
    case DMA1_Channel7_BASE:
        __HAL_RCC_DMA1_CLK_ENABLE();
        break;
    };
#endif

#ifdef DMA2
    switch ( (uint32_t)dma ) {
    case DMA2_Channel1_BASE:
    case DMA2_Channel2_BASE:
    case DMA2_Channel3_BASE:
    case DMA2_Channel4_BASE:
    case DMA2_Channel5_BASE:
    case DMA2_Channel6_BASE:
    case DMA2_Channel7_BASE:
        __HAL_RCC_DMA2_CLK_ENABLE();
        break;
    };
#endif
}

void i2c::dma_irq_on ( DMA_Channel_TypeDef* dma ) const {
#ifdef DMA1
    switch ( (uint32_t)dma ) {
    case DMA1_Channel1_BASE: NVIC_SetPriority( DMA1_Channel1_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel1_IRQn ); break;
    case DMA1_Channel2_BASE: NVIC_SetPriority( DMA1_Channel2_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel2_IRQn ); break;
    case DMA1_Channel3_BASE: NVIC_SetPriority( DMA1_Channel3_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel3_IRQn ); break;
    case DMA1_Channel4_BASE: NVIC_SetPriority( DMA1_Channel4_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel4_IRQn ); break;
    case DMA1_Channel5_BASE: NVIC_SetPriority( DMA1_Channel5_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel5_IRQn ); break;
    case DMA1_Channel6_BASE: NVIC_SetPriority( DMA1_Channel6_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel6_IRQn ); break;
    case DMA1_Channel7_BASE: NVIC_SetPriority( DMA1_Channel7_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel7_IRQn ); break;
    };
#endif

#ifdef DMA2
    switch ( (uint32_t)dma ) {
    case DMA2_Channel1_BASE: NVIC_SetPriority( DMA1_Channel1_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel1_IRQn ); break;
    case DMA2_Channel2_BASE: NVIC_SetPriority( DMA1_Channel2_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel2_IRQn ); break;
    case DMA2_Channel3_BASE: NVIC_SetPriority( DMA1_Channel3_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel3_IRQn ); break;
    case DMA2_Channel4_BASE: NVIC_SetPriority( DMA1_Channel4_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel4_IRQn ); break;
    case DMA2_Channel5_BASE: NVIC_SetPriority( DMA1_Channel5_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel5_IRQn ); break;
    case DMA2_Channel6_BASE: NVIC_SetPriority( DMA1_Channel6_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel6_IRQn ); break;
    case DMA2_Channel7_BASE: NVIC_SetPriority( DMA1_Channel7_IRQn, 6 );    NVIC_EnableIRQ( DMA1_Channel7_IRQn ); break;
    };
#endif

}

extern "C" {

void HAL_I2C_MasterTxCpltCallback( I2C_HandleTypeDef *hi2c ) {
    i2c* obj = ( i2c* )hi2c->obj;
    obj->s_give();
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
    i2c* obj = ( i2c* )hi2c->obj;
    obj->s_give();
}

}
