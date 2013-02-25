/************************************************
 * rtos.h
 *
 * Jason Gassel
 *
 * Defines functionality of the RT OS.
 ************************************************/

#ifndef RTOS_H
#define RTOS_H

// Initalize the RT OS
void os_init(void);

// Add an application to be run
//void os_app(???);

// Runs applications each tick
void os_tick_ISR(void);

#endif //RTOS_H

