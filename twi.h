/************************************************
 * twi.h
 *
 * Jason Gassel
 ************************************************/

#ifndef TWI_H
#define TWI_H

#include "main.h"

uint8 twi_write(uint8 device_addr, uint8 num_bytes, uint8 idata* send_array);
uint8 twi_read(uint8 device_addr, uint8 num_bytes, uint8 idata* read_array);

#endif
