/*
 * power_mod.h
 *
 * Created: 11/4/2017 1:14:31 PM
 *  Author: 
 */ 


#ifndef TEMPER_MOD_H_
#define TEMPER_MOD_H_
#include <asf.h>


typedef struct {
	struct rtc_module rtc_instance;
	unsigned int timeout;
	void (*timer_cb)(void);
} snrtimer_t;

int init_snrtimer(snrtimer_t *tm);
int start_snrtimer(snrtimer_t *tm);
int stop_snrtimer(snrtimer_t *tm);

#endif