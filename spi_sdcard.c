/************************************************
 * spi_sdcard.c
 *
 * Jason Gassel
 ************************************************/

#include "spi_sdcard.h"

void spi_sdcard_init(void)
{
  uint8 i, response[5];
  bit error = 0;
  
  SPCON = 0x73;
  
  // Send 72 clock pulses
  nCS = 1;
  for(i=0; i<71; ++i)
  {
    SPDAT = 0xFF;
    while((SPSTA & 0x80) != 0x80);
  }
  
  // Send CMD0
  spi_sdcard_command(0, 0);
  error = spi_sdcard_response(1, response);
  
  if(error)
    redLED = 0;
}

void spi_sdcard_command(uint8 cmd, uint32 arg)
{
  uint8 checkSum = 0x01;
  
  greenLED = 0;
  
  // Checksum
  if(cmd == 0)
  {
    checkSum = 0x95;
    arg = (0UL);
  }
  
  cmd &= 0x7F;
  cmd |= 0x40;
  
  nCS = 0;
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
  while((SPSTA & 0x80) != 0x80);
  
  greenLED = 1;
}

uint8 spi_sdcard_response(uint8 numBytes, uint8* buffer)
{
  uint8 in, i;
  uint8* current;
  uint8 error = 0;
  
  current = buffer;
  
  amberLED = 0;
  
  do
  {
    SPDAT = 0xFF;
    while((SPSTA & 0x80) != 0x80);
    in = SPDAT;
  } while(in == 0xFF);
  
  *current = in;
  current++;
  
  if(in == 0x01 || in == 0x00)
  {
    numBytes--;
    if(numBytes != 0)
    {
      for(i=0; i<numBytes; ++i)
      {
        SPSTA = 0xFF;
        while((SPSTA & 0x80) != 0x80);
        *current = SPDAT;
        current++;
      }
    }
  }
  else
      error = 1;
    
  SPDAT = 0xFF;
  
  nCS = 1;
  amberLED = 1;
  
  return error;
}
