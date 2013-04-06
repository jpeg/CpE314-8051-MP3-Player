/************************************************
 * filesystem.h
 *
 * Jason Gassel
 ************************************************/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "main.h"

// Utility functions
uint8 read8(uint16 offset, uint8 xdata* array);
uint16 read16(uint16 offset, uint8 xdata* array);
uint32 read32(uint16 offset, uint8 xdata* array);

#endif //FILESYSTEM_H
