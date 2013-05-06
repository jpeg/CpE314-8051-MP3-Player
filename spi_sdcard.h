/************************************************
 * spi_sdcard.h
 *
 * Jason Gassel
 ************************************************/

#ifndef SPI_SDCARD_H
#define SPI_SDCARD_H

#include "main.h"
#include "ports.h"
#include "uart.h"

#define SDCARD 0
#define MP3 1

void spi_sdcard_init(void);

void spi_sdcard_command(uint8 cmd, uint32 arg);

uint8 spi_sdcard_response(uint8 numBytes, uint8 idata* buffer);

uint8 spi_sdcard_block(uint16 numBytes, uint8* buffer);

void spi_mp3_data(uint16 numBytes, uint8 xdata* buffer);

#endif //SPI_SDCARD_H
