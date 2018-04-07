#include "rcc.h"

RCC_RESULT rcc::set_cfg ( uint32_t number_cfg_set ) const {
    if ( number_cfg_set >= this->number_cfg ) return RCC_RESULT::ERROR_CFG_NUMBER;

    HAL_RCC_DeInit();

    RCC_OscInitTypeDef*		osc = ( RCC_OscInitTypeDef* )&this->cfg[ number_cfg_set ].osc;
    RCC_ClkInitTypeDef*		clk = ( RCC_ClkInitTypeDef* )&this->cfg[ number_cfg_set ].clk;

	if ( HAL_RCC_OscConfig( osc ) != HAL_OK )
		return RCC_RESULT::ERROR_OSC_INIT;

	if ( HAL_RCC_ClockConfig( clk, this->cfg[ number_cfg_set ].f_latency ) != HAL_OK )
		return  RCC_RESULT::ERROR_CLK_INIT;

    return RCC_RESULT::OK;
}
