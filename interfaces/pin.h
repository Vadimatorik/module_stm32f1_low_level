#pragma once

#ifdef __cplusplus

#include "stm32f1xx_hal_conf.h"
#include "mc_hardware_interfaces_pin.h"

#ifdef HAL_GPIO_MODULE_ENABLED

#define PIN_COUNT(x) sizeof(x)/sizeof(x[0])

// Ответы от функций reinit_all, reinit_port объекта global_port.
enum class EC_ANSWER_GP {
    SUCCESS = 0,    // Переинициализация была успешной.
    LOOK    = 1     // Порт заблокирован, переинициализация некоторых
                    // (или всех) выводов невозможна.
};


// Структуры должны быть объявлены в extern "C" области или C файле.
struct pin_cfg {
    GPIO_TypeDef*               const GPIOx;
    const GPIO_InitTypeDef      init;
    uint32_t                    remap_mode;          // @ref Bit definition for AFIO_MAPR register. Серия AFIO_MAPR_
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

class pin_multifunc : public pin_multifunc_base {
public:
    pin_multifunc ( const pin_cfg* const cfg, uint32_t	number ): cfg( cfg ), number( number ) {}
    bool	init		( uint32_t number_cfg = 0 ) const;	// Перед инициализацией включается тактирование портов.
                                                            // Вызвать при первой инициализации.

    bool    reinit      ( uint32_t number_cfg = 0 ) const;

    void    set         ( void )   			const;
    void    reset       ( void )   			const;

    void	set			( bool state )      const;
    void    set         ( int state )       const;
    void    set         ( uint32_t state )  const;

    void	toggle		( void )            const;
    bool	read		( void )            const;


protected:
    const	pin_cfg*    const		cfg;				// Массив структур переинициализации вывода.
    const 	uint32_t	number;
};

class pin_multifunc_it : public pin_multifunc, public pin_multifunc_it_base {
public:
    bool check_it   (void) const;
    void clear_it   (void) const;
};

class global_port : public global_port_base {
public:
    global_port( const pin_cfg* const cfg_array, uint32_t number_pin ) : cfg( cfg_array ), number_pin( number_pin ) {}
    void reinit_all_ports ( void ) const;

private:
    const	pin_cfg*    const		cfg;
    const 	uint32_t	number_pin;
};

#endif

#endif
