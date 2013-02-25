/************************************************
 * rtos.c
 *
 * Jason Gassel
 ************************************************/
 #include "rtos.h"

void os_init(void)
{
	//TODO parameter for tick rate
	//TODO init timer
}

void os_tick_ISR(void) interrupt 3 using 3
{
	//TODO reset timer

	//TODO run apps
}

