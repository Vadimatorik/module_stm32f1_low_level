#pragma once

#include "mc_hardware_interfaces_i2c.h"

#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx_hal_dma.h"
#include "stm32f1xx_hal_rcc.h"

#include "user_os.h"

struct i2c_cfg {
    I2C_TypeDef*            i2c_inst;
    uint32_t                speed;                             //  Гц, до 400000 Гц
    DMA_Channel_TypeDef*    dma_tx;
    DMA_Channel_TypeDef*    dma_rx;
};

class i2c : public i2c_base {
public:
    i2c( const i2c_cfg* const cfg );

    void reinit     ( void ) const;

    int read        ( uint8_t slave_addr, uint8_t* p_buf, uint8_t read_addr, uint16_t num_byte_to_readr ) const;
    int read_dma    ( uint8_t slave_addr, uint8_t* p_buf, uint8_t read_addr, uint16_t num_byte_to_readr ) const;
    int write_byte  ( uint8_t slave_addr, uint8_t* p_buf, uint8_t write_addr ) const;

    void s_give ( void ) const;

private:
    void init_clk_i2c ( void ) const;
    void init_i2c_irq ( void ) const;
    void dma_clk_on ( DMA_Channel_TypeDef* dma) const;
    void dma_irq_on ( DMA_Channel_TypeDef* dma) const;


    const i2c_cfg* const cfg;

    mutable USER_OS_STATIC_BIN_SEMAPHORE            s             = nullptr;
    mutable USER_OS_STATIC_BIN_SEMAPHORE_BUFFER     sb;

    mutable I2C_HandleTypeDef handle;

    mutable DMA_HandleTypeDef dma_tx;
    mutable DMA_HandleTypeDef dma_rx;
};
