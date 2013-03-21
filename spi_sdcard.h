/************************************************
 * spi_sdcard.h
 *
 * Jason Gassel
 ************************************************/

#ifndef SPI_SDCARD_H
#define SPI_SDCARD_H

#include "main.h"
#include "ports.h"

void spi_sdcard_init(void);

void spi_sdcard_command(uint8 cmd, uint32 arg);

uint8 spi_sdcard_response(uint8 numBytes, uint8* buffer);

#endif //SPI_SDCARD_H
