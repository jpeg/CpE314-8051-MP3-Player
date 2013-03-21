/************************************************
 * spi_sdcard.c
 *
 * Jason Gassel
 ************************************************/

#include "spi_sdcard.h"

void spi_sdcard_init(void)
{
  SPCON = 0x5D;
  
  // Send CMD0
  spi_sdcard_command(0, 0);
}

void spi_sdcard_command(uint8 cmd, uint32 arg)
{
  uint8 checkSum = 0x01;
  static bit first = 0;
  
  greenLED = 0;
  
  // Checksum
  if(cmd == 0)
  {
    checkSum = 0x9A;
    arg = (0UL);
  }
  
  cmd &= 0x7F;
  cmd |= 0x40;
  
  nCS = 0;
  if(first)
    while((SPSTA & 0x80) != 0x80);
  else
    first = 1;
  SPDAT = cmd;
  while((SPSTA & 0x80) != 0x80);
  SPDAT = arg >> 24;
  while((SPSTA & 0x80) != 0x80);
  SPDAT = (arg >> 16) & 0xFFUL;
  while((SPSTA & 0x80) != 0x80);
  SPDAT = (arg >> 8) & 0xFFUL;
  while((SPSTA & 0x80) != 0x80);
  SPDAT = arg & 0xFFUL;
  while((SPSTA & 0x80) != 0x80);
  SPDAT = checkSum;
  nCS = 1;
  
  greenLED = 1;
}
