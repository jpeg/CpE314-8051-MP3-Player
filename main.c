/************************************************
 * main.c
 *
 * Jason Gassel
 ************************************************/

#include "main.h"
#include "ports.h"
#include "rtos.h"

// Tasks
void blinkLED(void);

void main(void)
{
    //Initialize
    rtos_init();
    rtos_task(&blinkLED, 1000);
    
    // Run
	rtos_spin();
}

void blinkLED(void)
{
    static bit state = 1;
    
    greenLED = state;
    state = ~state;
}
