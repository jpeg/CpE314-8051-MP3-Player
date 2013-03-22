/************************************************
 * main.h
 *
 * Jason Gassel
 ************************************************/

#ifndef MAIN_H
#define MAIN_H

#include <AT89C51RC2.h>

sfr SPCON = 0xC3;
sfr SPSTA = 0xC4;
sfr SPDAT = 0xC5;

#define OSC_FREQ (11059200UL)
#define OSC_PER_INST (6)
#define BAUDRATE (9600UL)
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;

#endif //MAIN_H
