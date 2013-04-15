/************************************************
 * filesystem.h
 *
 * Jason Gassel
 ************************************************/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "main.h"
#include "spi_sdcard.h"
#include "uart.h"

void fs_init();

uint32 fs_selectDirectoryEntry(uint32 startCluster);

void fs_longFilename(uint32 cluster);

void fs_swapBuffer();

// Utility functions
uint8 read8(uint16 offset, uint8 xdata* array);
uint16 read16(uint16 offset, uint8 xdata* array);
uint32 read32(uint16 offset, uint8 xdata* array);

#endif //FILESYSTEM_H
