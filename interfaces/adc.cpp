#include "adc.h"

adc_one_channel::adc_one_channel( const adc_one_channel_cfg* const cfg ) {
	this->adc_st.Instance					= cfg->ADCx;
	this->adc_st.Init.DataAlign				= cfg->data_align;
	this->adc_st.Init.ScanConvMode			= ENABLE;
	this->adc_st.Init.ContinuousConvMode	= ENABLE;
	this->adc_st.Init.NbrOfConversion		= 1;

	this->channel_cfg.Channel				= cfg->channel;
	this->channel_cfg.Rank					= 1;
	this->channel_cfg.SamplingTime			= cfg->sampling_time;
}

bool adc_one_channel::reinit ( void ) const {
	this->clk_enable();
	HAL_ADC_DeInit( &this->adc_st );

	HAL_StatusTypeDef r;
	r = HAL_ADC_Init( &this->adc_st );
	if ( r != HAL_OK ) return false;
	r = HAL_ADC_ConfigChannel( &this->adc_st, &this->channel_cfg );
	if ( r != HAL_OK ) return false;

	return true;
}

void adc_one_channel::clk_enable ( void ) const {
	switch( ( uint32_t )this->adc_st.Instance ) {
#ifdef ADC1
		case ADC1_BASE: __HAL_RCC_ADC1_CLK_ENABLE(); break;
#endif

#ifdef ADC2
		case ADC2_BASE: __HAL_RCC_ADC2_CLK_ENABLE(); break;
#endif

#ifdef ADC3
		case ADC3_BASE: __HAL_RCC_ADC3_CLK_ENABLE(); break;
#endif
	}
}

void adc_one_channel::clk_disable ( void ) const {
	switch( ( uint32_t )this->adc_st.Instance ) {
#ifdef ADC1
		case ADC1_BASE:	__HAL_RCC_ADC1_CLK_DISABLE(); break;
#endif

#ifdef ADC2
		case ADC2_BASE:	__HAL_RCC_ADC2_CLK_DISABLE(); break;
#endif

#ifdef ADC3
		case ADC3_BASE:	__HAL_RCC_ADC3_CLK_DISABLE(); break;
#endif
	}
}

bool adc_one_channel::start_continuous_conversion ( void ) const {
	HAL_StatusTypeDef r;
	r = HAL_ADC_Start( &this->adc_st );
	if ( r != HAL_OK ) return false;
	return true;
}

bool adc_one_channel::stop_continuous_conversion ( void ) const {
	HAL_StatusTypeDef r;
	r = HAL_ADC_Stop( &this->adc_st );
	if ( r != HAL_OK ) return false;
	return true;
}

void adc_one_channel::get_measurement ( uint32_t& channel_measurement ) const {
	channel_measurement = HAL_ADC_GetValue( &this->adc_st );
}
