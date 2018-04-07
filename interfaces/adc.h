#include "mc_hardware_interfaces_adc.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_rcc.h"

struct adc_one_channel_cfg {
	ADC_TypeDef*		ADCx;
	uint32_t			clock_prescaler;							// ADC_ClockPrescaler
	uint32_t			resolution;									// ADC_Resolution
	uint32_t			data_align;									// ADC_Data_align
	uint32_t			channel;									// ADC_channels
	uint32_t			sampling_time;								// ADC_sampling_times
};

class adc_one_channel : public adc_one_channel_base {
public:
	adc_one_channel( const adc_one_channel_cfg* const cfg );

	bool reinit ( void )													const;
	bool start_continuous_conversion ( void )								const;
	bool stop_continuous_conversion ( void )								const;
	void get_measurement ( uint32_t& channel_measurement )					const;

	void clk_enable ( void )												const;
	void clk_disable ( void )												const;

private:
	mutable ADC_HandleTypeDef				adc_st;
	mutable ADC_ChannelConfTypeDef			channel_cfg;
};
