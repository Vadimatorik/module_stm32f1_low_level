#pragma once

#ifdef __cplusplus

#include "stm32f1xx_hal_conf.h"
#include "mc_hardware_interfaces_pin.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#define PIN_COUNT(x) sizeof(x)/sizeof(x[0])

// Структуры должны быть объявлены в extern "C" области или C файле.
struct pin_cfg {
    const GPIO_TypeDef*         const GPIOx;
    const GPIO_InitTypeDef      init;
};

class pin : public pin_base {
public:
    pin( const pin_cfg* const cfg ) : cfg( cfg ) {}

    void	init    ( void )            const;                        // Перед инициализацией включается тактирование портов.

    void    set     ( void )            const;
    void    reset   ( void )            const;
    void	toggle	( void )            const;

    void    set     ( bool state )      const;
    void    set     ( int state )       const;
    void    set     ( uint8_t state )   const;

    bool    read    ( void )            const;

private:
    const pin_cfg*      const cfg;

};

class pin_multifunc : public pin, public pin_multifunc_base {
public:
    pin_multifunc( const pin_cfg* const cfg, uint32_t count_cfg ) : pin( cfg ), count_cfg( count_cfg ) {}
    bool    reinit  ( uint32_t number_cfg )  const;

private:
    const uint32_t      count_cfg;
};

void gpio_clk_en (  const GPIO_TypeDef* const GPIOx );

#endif

#endif
