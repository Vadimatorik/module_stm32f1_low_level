#pragma once

#ifdef __cplusplus

#include "pin.h"

#ifdef HAL_GPIO_MODULE_ENABLED

class global_port : public global_port_base {
public:
    global_port( const pin_cfg* const cfg, uint32_t number_cfg ) : cfg( cfg ), number_cfg( number_cfg ) {}
    void reinit_all_ports ( void ) const;

private:
    const pin_cfg* const cfg;
    const uint32_t number_cfg;
};

#endif

#endif
