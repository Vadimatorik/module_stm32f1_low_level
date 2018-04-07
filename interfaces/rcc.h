#include "mc_hardware_interfaces_rcc.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal_flash.h"
#include <stdint.h>

struct rcc_cfg {
	RCC_ClkInitTypeDef			clk;
	RCC_OscInitTypeDef			osc;
	uint32_t					f_latency;
};

class rcc : public rcc_base {
public:
    rcc( const rcc_cfg* const cfg, const uint32_t number_cfg ) : cfg( cfg ), number_cfg( number_cfg ) {}
    RCC_RESULT set_cfg ( uint32_t number_cfg_set ) const;

private:
    const rcc_cfg*              const cfg;
    const uint32_t              number_cfg;
};
