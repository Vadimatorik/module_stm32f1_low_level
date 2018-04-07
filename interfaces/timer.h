#pragma once

#include "mc_hardware_interfaces_timer.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_rcc.h"

/*!
 * Параметры делителя и вершины.
 * Для формирования массива конфигураций
 * таймера в зависимости от выбранной
 * частоты RCC.
 */
struct clk_tim_param {
	const uint16_t					period;					// 0..0xFFFF.
	const uint16_t					prescaler;				// 0..0xFFFF.

	/// Используется только в:
	/// 1. tim_comp_one_channel_cfg
	const uint16_t					pulse;					// 0..0xFFFF.
};

/*!
 * Таймер предназначен для генерации прямоугольного сигнала.
 */
struct tim_comp_one_channel_cfg {
	// Используемый таймер.
	TIM_TypeDef*				tim;

	// Параметры таймера.
	HAL_TIM_ActiveChannel		tim_channel;

	const clk_tim_param*		const p_cfg;
	uint32_t					size_cfg;

	// Параметры выходного канала.
	uint8_t						out_channel;			// TIM_CHANNEL_x.
	uint32_t					polarity;				// TIM_OCPOLARITY_LOW  / TIM_OCPOLARITY_HIGH.

};

class tim_comp_one_channel : public tim_comp_one_channel_base {
public:
	tim_comp_one_channel( const tim_comp_one_channel_cfg* const cfg );
	bool reinit ( uint32_t cfg_number ) const;
	bool on ( void ) const;
	bool off ( void ) const;

private:
	const tim_comp_one_channel_cfg*		const cfg;
	mutable TIM_HandleTypeDef			hal_tim_cfg;
	mutable	TIM_OC_InitTypeDef			hal_tim_ch_cfg;
};

//**********************************************************************
// tim_pwm_one_channel
//**********************************************************************
struct tim_pwm_one_channel_cfg {
	// Используемый таймер.
	TIM_TypeDef*				tim;

	// Параметры таймера.
	HAL_TIM_ActiveChannel		tim_channel;

	const clk_tim_param*		const p_cfg;
	uint32_t					size_cfg;

	// Параметры выходного канала.
	uint8_t						out_channel;			// TIM_CHANNEL_x.
	uint32_t					polarity;				// TIM_OCPOLARITY_LOW  / TIM_OCPOLARITY_HIGH.
};

class tim_pwm_one_channel : public tim_pwm_one_channel_base {
public:
	tim_pwm_one_channel( const tim_pwm_one_channel_cfg* const cfg );
	bool reinit ( uint32_t cfg_number ) 			const;
	bool on ( void ) 				const;
	bool off ( void ) 				const;
	void duty_set ( float duty ) 	const;

private:
	const tim_pwm_one_channel_cfg*		const cfg;
	mutable TIM_HandleTypeDef			hal_tim_cfg;
	mutable	TIM_OC_InitTypeDef			hal_tim_ch_cfg;
};

//**********************************************************************
// tim_interrupt
//**********************************************************************
struct tim_interrupt_cfg {
	TIM_TypeDef*				tim;

	HAL_TIM_ActiveChannel		tim_channel;

	const clk_tim_param*		const p_cfg;
	uint32_t					size_cfg;
};

class tim_interrupt : public tim_interrupt_base {
public:
	tim_interrupt( const tim_interrupt_cfg* const cfg );

	bool reinit					( uint32_t cfg_number ) const;
	bool on						( void ) const;
	bool off					( void ) const;
	void clear_interrupt_flag	( void ) const;

private:
	const tim_interrupt_cfg*		const cfg;
	mutable TIM_HandleTypeDef		hal_tim_cfg;
};
