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
//#include "uart.h"
#include "spi_sdcard.h"
#include "twi.h"
#include "filesystem.h"

#define RTOS_READ_FAT   1
#define RTOS_READ_BUF   2
#define RTOS_SEND_DATA  3
#define RTOS_CHECK_MEM  4
#define RTOS_READ_SEC   5
#define RTOS_NEXT_CLUS  6
#define RTOS_LOCATE_SEC 7

// Initalize the RT OS
void rtos_init(void);

// Run the background print to serial task
void rtos_spin();

// Runs applications each tick, uses timer2
void rtos_tick_ISR(void);

#endif //RTOS_H
