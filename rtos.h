/************************************************
 * rtos.h
 *
 * Jason Gassel
 *
 * Defines functionality of the RT OS.
 ************************************************/

#ifndef RTOS_H
#define RTOS_H

#include "main.h"
#include "ports.h"
#include "uart.h"
#include "spi_sdcard.h"

#define RTOS_MAX_NUM_TASKS (2)
typedef void (*task_type)(void);

// Initalize the RT OS
void rtos_init(void);

// Add an application to be run, msec sets how many milliseconds between runs
void rtos_task(task_type task, uint16 msec);

// Changes the rate at which a task runs
//void rtos_rate(task_type task, uint16 msec);

// Gets the current rate for a task
//uint16 rtos_getRate(task_type task);

// Run the background print to serial task
void rtos_spin();

// Runs applications each tick, uses timer2
void rtos_tick_ISR(void);

#endif //RTOS_H
