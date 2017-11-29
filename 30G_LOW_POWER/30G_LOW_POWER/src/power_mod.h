/*
 * power_mod.h
 *
 * Created: 11/4/2017 1:14:31 PM
 *  Author: 
 */ 


#ifndef POWER_MOD_H_
#define POWER_MOD_H_


#define CONF_EXT_WAKEUP_PIN   PIN_PA00A_RSTC_EXTWAKE0
#define CONF_STDIO_PAD0_PIN PIN_PA22C_SERCOM3_PAD0
#define CONF_STDIO_PAD1_PIN PIN_PA23C_SERCOM3_PAD1
//#define PWRMOD_IDLE	0
//#define PWRMOD_ACTIVE	1

typedef enum { 
PWRMOD_IDLE   = 0, 
PWRMOD_BACKUP = 1,
PWRMOD_ACTIVE = 2,
PWRMOD_SL_OFF =3

} pwrmod_t;


int init_power_mod(void);
static void set_power_mod(pwrmod_t mod);


#endif /* POWER_MOD_H_ */