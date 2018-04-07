#pragma once

#ifdef __cplusplus

#include "mc_hardware_interfaces_wdt.h"
#include "stm32f1xx_hal_conf.h"
#include "user_os.h"

struct wdt_cfg {
	const uint8_t	task_prio;				// Приоритет задачи, сбрасывающий wdt.
	const uint32_t	run_time_ms;			// Время перезагрузки по сторожевому таймеру.
											// при номинальном режиме работы системы.
	const uint32_t	startup_time_ms;		// Время перезагрузки по сторожевому таймеру при запуске системы.
	const uint32_t	service_time_ms;		// Время перезагрузки по сторожевому таймеру
											// во время сервисных операций (например, стирание и перезапись flash).
};

class wdt : public wdt_base {
public:
	wdt ( const wdt_cfg* const cfg ) : cfg( cfg ) {}

	void init ( void )			const;
	void reset ( void )			const;
	void reset_service ( void ) const;

private:
	const wdt_cfg*							const cfg;

	static void task ( void* p_obj );

	mutable USER_OS_STATIC_STACK_TYPE		task_stack[ 64 ] = { 0 };
	mutable USER_OS_STATIC_TASK_STRUCT_TYPE	task_struct;

	uint8_t									reboot;
};

#endif
