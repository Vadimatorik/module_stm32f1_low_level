#include "pin.h"

//**********************************************************************
// Включение тактирования порта.
//**********************************************************************
static void gpio_clk_en (  const GPIO_TypeDef* GPIOx, const uint32_t remap_mode ) {
    if ( remap_mode != 0 ) {
        __HAL_RCC_AFIO_CLK_ENABLE();
        SET_BIT( AFIO->MAPR, remap_mode );
    }

    switch ( (uint32_t)GPIOx ) {
#ifdef GPIOA
        case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;
#endif

#ifdef GPIOB
        case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;
#endif

#ifdef GPIOC
        case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;
#endif

#ifdef GPIOD
        case GPIOD_BASE: __HAL_RCC_GPIOD_CLK_ENABLE(); break;
#endif

#ifdef GPIOE
        case GPIOE_BASE: __HAL_RCC_GPIOE_CLK_ENABLE(); break;
#endif

#ifdef GPIOF
        case GPIOF_BASE: __HAL_RCC_GPIOF_CLK_ENABLE(); break;
#endif

#ifdef GPIOG
        case GPIOG_BASE: __HAL_RCC_GPIOG_CLK_ENABLE(); break;
#endif

#ifdef GPIOH
        case GPIOH_BASE: __HAL_RCC_GPIOH_CLK_ENABLE(); break;
#endif
    }
}

//**********************************************************************
// PIN.
//**********************************************************************
void pin::init ( void ) const {
    gpio_clk_en( this->cfg->GPIOx, this->cfg->remap_mode );
    HAL_GPIO_Init( this->cfg->GPIOx, ( GPIO_InitTypeDef * )this->cfg );
}

void pin::set ( void ) const {
    HAL_GPIO_WritePin ( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin, GPIO_PIN_SET);
}

void pin::reset ( void ) const {
    HAL_GPIO_WritePin ( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin, GPIO_PIN_RESET);
}

void pin::toggle ( void ) const {
    HAL_GPIO_TogglePin ( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin );
}

void pin::set ( bool state ) const {
    if ( state ) {
        this->set();
    } else {
        this->reset();
    }
}

void pin::set ( int state ) const {
    this->set( static_cast< bool >( state ) );
}

void pin::set ( uint8_t state ) const {
    this->set( static_cast< bool >( state ) );
}

bool pin::read ( void ) const {
    return HAL_GPIO_ReadPin ( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin );
}

//**********************************************************************
// MULTIFUNC.
//**********************************************************************
bool pin_multifunc::init ( uint32_t number_cfg ) const {
    if ( number_cfg >= this->number ) return false;
    gpio_clk_en( this->cfg[number_cfg].GPIOx, this->cfg[number_cfg].remap_mode );
    this->reinit( number_cfg );             // Инициализируем
    return true;
}

bool pin_multifunc::reinit ( uint32_t number_cfg ) const {
    if ( number_cfg >= this->number ) return false;
    HAL_GPIO_Init( cfg[number_cfg].GPIOx, (GPIO_InitTypeDef *) &cfg[number_cfg].init );
    return true;
}

void pin_multifunc::set ( void ) const {
    HAL_GPIO_WritePin ( this->cfg->GPIOx, (uint16_t) this->cfg->init.Pin, GPIO_PIN_SET);
}

void pin_multifunc::reset ( void ) const {
    HAL_GPIO_WritePin ( this->cfg->GPIOx, (uint16_t) this->cfg->init.Pin, GPIO_PIN_RESET);
}

void pin_multifunc::set ( bool state ) const {
    if ( state ) {
        this->set();
    } else {
        this->reset();
    }
}

void pin_multifunc::set ( int state ) const {
    this->set( static_cast< bool >( state ) );
}

void pin_multifunc::set ( uint32_t state ) const {
    this->set( static_cast< bool >( state ) );
}

void pin_multifunc::toggle ( void ) const {
    HAL_GPIO_TogglePin( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin );
}

bool pin_multifunc::read ( void ) const {
    return HAL_GPIO_ReadPin ( this->cfg->GPIOx, ( uint16_t )this->cfg->init.Pin );
}

//**********************************************************************
// MULTIFUNC_IT.
//**********************************************************************
bool pin_multifunc_it::check_it ( void ) const {
    return static_cast< bool >( __HAL_GPIO_EXTI_GET_IT( this->cfg->init.Pin ) );
}

void pin_multifunc_it::clear_it ( void ) const {
    __HAL_GPIO_EXTI_CLEAR_IT( this->cfg->init.Pin );
}

//**********************************************************************
// GLOBAL_PORT.
//**********************************************************************
void global_port::reinit_all_ports ( void ) const {
    for ( uint32_t l = 0; l < this->number_pin; l++ ) {
        gpio_clk_en( this->cfg[l].GPIOx, this->cfg[l].remap_mode );
        HAL_GPIO_Init( cfg[l].GPIOx, ( GPIO_InitTypeDef* ) ( &cfg[l].init ) );
    }
}
