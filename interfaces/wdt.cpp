#include "wdt.h"

void wdt::init ( void ) const {
    *((uint32_t *) 0xE0042008)  = 0x1DFF;				// Если процессор находится в состоянии halt, watchdog будет стоять и не мешать отладке.

    IWDG->KR = 0xCCCC;
    IWDG->KR= 0x5555;
    IWDG->PR = 0x6;
    IWDG->RLR = 32 * this->cfg->startup_time_ms / 256;

    while( IWDG->SR );

    IWDG->KR = 0xAAAA;                                   // Перезагружаем WDT.

    USER_OS_STATIC_TASK_CREATE( this->task, "wdt_th", 64, ( void* )this, this->cfg->task_prio, this->task_stack, &this->task_struct );
}

void wdt::reset ( void ) const {
	IWDG->KR = 0x5555;//ключ, разрешающий запись
	IWDG->RLR = 40 * this->cfg->run_time_ms / 256;
	IWDG->KR = 0xAAAA;//ключ, перезагружающий таймер

}

// Перезапуск таймера для сервисных операций
void wdt::reset_service ( void ) const {
	IWDG->KR = 0x5555;//ключ, разрешающий запись
	IWDG->RLR = 40 * this->cfg->service_time_ms / 256;
	IWDG->KR = 0xAAAA;//ключ, перезагружающий таймер
}

//**********************************************************************
// Потоки.
//**********************************************************************
void wdt::task ( void* p_obj ) {
    wdt* obj = ( wdt * )p_obj;
    USER_OS_TICK_TYPE last_wake_time;
    const USER_OS_TICK_TYPE time_out = 10;
    last_wake_time = USER_OS_TASK_GET_TICK_COUNT();
    while ( true ) {
        configASSERT( !obj->reboot );					// Если задана перезагрузка - виснем!
        obj->reset();
        USER_OS_TASK_DELAY_UNTIL( &last_wake_time, time_out );
    }
}
