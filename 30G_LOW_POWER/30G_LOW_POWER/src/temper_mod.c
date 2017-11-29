#include <asf.h>
#include "timer_mod.h"

//#define RTC_CLOCK_SOURCE    RTC_CLOCK_SELECTION_ULP1K

int init_temper()
{
	 /* Configure device and enable. */
	 at30tse_init();
	return 0;
}

double temp_res()
{
    /* Read thigh and tlow */
    volatile uint16_t thigh = 0;
    thigh = at30tse_read_register(AT30TSE_THIGH_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_THIGH_REG_SIZE);
    volatile uint16_t tlow = 0;
    tlow = at30tse_read_register(AT30TSE_TLOW_REG, AT30TSE_NON_VOLATILE_REG, AT30TSE_TLOW_REG_SIZE);

    /* Set 12-bit resolution mode. */
    at30tse_write_config_register(AT30TSE_CONFIG_RES(AT30TSE_CONFIG_RES_12_bit));

    while (1) {
	    temp_res = at30tse_read_temperature();
    }

	return temp_res;
}
