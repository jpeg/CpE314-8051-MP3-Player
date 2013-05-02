/************************************************
 * twi.h
 *
 * Jason Gassel
 ************************************************/

#ifndef TWI_H
#define TWI_H

//#include "main.h"
typedef unsigned char uint8;

uint8 twi_write(uint8 device_addr, uint8 num_bytes, uint8* send_array);
uint8 twi_read(uint8 device_addr, uint8 num_bytes, uint8* read_array);

#endif
