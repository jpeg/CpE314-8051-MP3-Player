/************************************************
 * filesystem.h
 *
 * Jason Gassel
 ************************************************/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "main.h"
#include "spi_sdcard.h"
//#include "uart.h"

// Declare extern variables
extern uint32 idata fs_FATfirstDataSector;
extern uint32 idata fs_FATsectorsPerCluster;

void fs_init();

uint32 fs_FATentry(const uint32 cluster);

bit fs_FATeof(const uint32 cluster);

uint32 fs_findMP3(const uint32 startCluster);

void fs_loadSector(uint32 cluster, uint8 relativeSector);

void fs_setBuffer(bit buffer);

uint8 xdata* fs_currentBuffer();

// Utility functions
uint8 read8(uint16 offset, uint8 xdata* array);
uint16 read16(uint16 offset, uint8 xdata* array);
uint32 read32(uint16 offset, uint8 xdata* array);

#endif //FILESYSTEM_H
