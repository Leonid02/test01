/*
 * power_mod.c
 *
 * Created: 11/4/2017 1:13:51 PM
 *  Author: 
 */ 

#include <asf.h>
#include "power_mod.h"

/*
test
test
*/

///**
 //* \brief Main clock source selection between DFLL and OSC16M.
 //*/
//static void main_clock_select(const enum system_clock_source clock_source)
//{
	//if (clock_source == SYSTEM_CLOCK_SOURCE_DFLL) {
		//main_clock_select_dfll();
		//system_clock_source_disable(SYSTEM_CLOCK_SOURCE_OSC16M);
	//} else if (clock_source == SYSTEM_CLOCK_SOURCE_OSC16M) {
		//main_clock_select_osc16m();
		//system_clock_source_disable(SYSTEM_CLOCK_SOURCE_DFLL);
		//system_gclk_chan_disable(OSCCTRL_GCLK_ID_DFLL48);
		//system_gclk_gen_disable(GCLK_GENERATOR_1);
	//} else {
		//return ;
	//}
//}
///**
 //* \brief Setect DFLL as main clock source.
 //*/
//static void main_clock_select_dfll(void)
//{
	//struct system_gclk_gen_config gclk_conf;
//
	///* Select OSCULP32K as new clock source for mainclock temporarily */
	//system_gclk_gen_get_config_defaults(&gclk_conf);
	//gclk_conf.source_clock = SYSTEM_CLOCK_SOURCE_XOSC32K;
	//system_gclk_gen_set_config(GCLK_GENERATOR_0, &gclk_conf);
//
	///* Select XOSC32K for GCLK1. */
	//system_gclk_gen_get_config_defaults(&gclk_conf);
	//gclk_conf.source_clock = SYSTEM_CLOCK_SOURCE_XOSC32K;
	//system_gclk_gen_set_config(GCLK_GENERATOR_1, &gclk_conf);
	//system_gclk_gen_enable(GCLK_GENERATOR_1);
//
	//struct system_gclk_chan_config dfll_gclk_chan_conf;
//
	//system_gclk_chan_get_config_defaults(&dfll_gclk_chan_conf);
	//dfll_gclk_chan_conf.source_generator = GCLK_GENERATOR_1;
	//system_gclk_chan_set_config(OSCCTRL_GCLK_ID_DFLL48, &dfll_gclk_chan_conf);
	//system_gclk_chan_enable(OSCCTRL_GCLK_ID_DFLL48);
//
	//struct system_clock_source_dfll_config dfll_conf;
	//system_clock_source_dfll_get_config_defaults(&dfll_conf);
//
	//dfll_conf.loop_mode      = SYSTEM_CLOCK_DFLL_LOOP_MODE_CLOSED;
	//dfll_conf.on_demand      = false;
	//dfll_conf.run_in_stanby  = CONF_CLOCK_DFLL_RUN_IN_STANDBY;
	//dfll_conf.multiply_factor = CONF_CLOCK_DFLL_MULTIPLY_FACTOR;
	//system_clock_source_dfll_set_config(&dfll_conf);
	//system_clock_source_enable(SYSTEM_CLOCK_SOURCE_DFLL);
	//while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_DFLL));
	//if (CONF_CLOCK_DFLL_ON_DEMAND) {
		//OSCCTRL->DFLLCTRL.bit.ONDEMAND = 1;
	//}
//}
	///**
 //* \brief Setect OSC16M as main clock source.
 //*/
//static void main_clock_select_osc16m(void)
//{
	//struct system_gclk_gen_config gclk_conf;
	//struct system_clock_source_osc16m_config osc16m_conf;
//
	///* Switch to new frequency selection and enable OSC16M */
	//system_clock_source_osc16m_get_config_defaults(&osc16m_conf);
	//osc16m_conf.fsel = CONF_CLOCK_OSC16M_FREQ_SEL;
	//osc16m_conf.on_demand = 0;
	//osc16m_conf.run_in_standby = CONF_CLOCK_OSC16M_RUN_IN_STANDBY;
	//system_clock_source_osc16m_set_config(&osc16m_conf);
	//system_clock_source_enable(SYSTEM_CLOCK_SOURCE_OSC16M);
	//while(!system_clock_source_is_ready(SYSTEM_CLOCK_SOURCE_OSC16M));
//
	///* Select OSC16M as mainclock */
	//system_gclk_gen_get_config_defaults(&gclk_conf);
	//gclk_conf.source_clock = SYSTEM_CLOCK_SOURCE_OSC16M;
	//system_gclk_gen_set_config(GCLK_GENERATOR_0, &gclk_conf);
	//if (CONF_CLOCK_OSC16M_ON_DEMAND) {
		//OSCCTRL->OSC16MCTRL.reg |= OSCCTRL_OSC16MCTRL_ONDEMAND;
	//}
//
//}

 int init_power_mod(){

 	SUPC->BOD33.reg &= ~SUPC_BOD33_ENABLE;
 	#ifndef SAMR30
 	/* VDDCORE is supplied BUCK converter */
 	SUPC->VREG.bit.SEL = SUPC_VREG_SEL_BUCK_Val;
 	#endif
 	//delay_init();
 	//configure_usart();
 	//configure_extint_channel();

 	/* Turn LED0 ON */
 	port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);

 	if (system_get_reset_cause() == SYSTEM_RESET_CAUSE_BACKUP
 	&& system_get_backup_exit_source() == SYSTEM_RESET_BACKKUP_EXIT_EXTWAKE
 	&& (system_get_pin_wakeup_cause() & (1 << CONF_EXT_WAKEUP_PIN))){

		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
 	}
	return 1;
}

 
 static void set_power_mod(pwrmod_t mod){
 enum system_performance_level curr_pl = system_get_performance_level();

 /* Idled power mode*/
 switch (mod) {
 	case PWRMOD_IDLE:

		if (curr_pl!= SYSTEM_PERFORMANCE_LEVEL_0){
			system_switch_performance_level(SYSTEM_PERFORMANCE_LEVEL_0);
			///* Scaling up the performance level first and then increase clock frequency */
			//main_clock_select(SYSTEM_CLOCK_SOURCE_DFLL);

			port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
			system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE);
			system_sleep();
		}
		break;
	/* Activated power mode*/
  	case PWRMOD_ACTIVE:

 		if (curr_pl== SYSTEM_PERFORMANCE_LEVEL_0){
			///* Scaling down clock frequency and then Scaling down the performance level */
			//main_clock_select(SYSTEM_CLOCK_SOURCE_OSC16M);

			system_switch_performance_level(SYSTEM_PERFORMANCE_LEVEL_2);
			/* Toggles LED0  wake up from IDLE sleep mode */
			port_pin_toggle_output_level(LED_0_PIN);
		}
		break;
	/* Activated power mode*/
  	case PWRMOD_BACKUP:
		system_apb_clock_clear_mask(SYSTEM_CLOCK_APB_APBC, MCLK_APBCMASK_SERCOM3);
		system_gclk_chan_disable(SERCOM0_GCLK_ID_CORE + 3);

		struct port_config pin_conf;
		port_get_config_defaults(&pin_conf);

		pin_conf.direction = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(CONF_STDIO_PAD0_PIN, &pin_conf);
		port_pin_set_config(CONF_STDIO_PAD1_PIN, &pin_conf);

		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);

		/* Set external wakeup pin polarity */
		system_set_pin_wakeup_polarity_low(1 << CONF_EXT_WAKEUP_PIN);

		/* Set external wakeup detector */
		system_enable_pin_wakeup(1 << CONF_EXT_WAKEUP_PIN);
		system_set_pin_wakeup_debounce_counter(SYSTEM_WAKEUP_DEBOUNCE_2CK32);
		system_set_sleepmode(SYSTEM_SLEEPMODE_BACKUP);
		system_sleep();

		break;
  	case PWRMOD_SL_OFF:
			/* Enter OFF mode */
			system_set_sleepmode(SYSTEM_SLEEPMODE_OFF);
			system_sleep();
		break;
	default:
		break;
	}
 }
