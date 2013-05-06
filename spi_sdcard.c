/************************************************
 * spi_sdcard.c
 *
 * Jason Gassel
 ************************************************/

#include "spi_sdcard.h"

static bit spi_sdcard_standardCapacity = 0;

void spi_sdcard_init(void)
{
  uint8 i, response[16];
  uint32* response32 = &response[1];
  uint8 error = 0;
  
  SPCON = 0x73;
  
  // Send 72 clock pulses
  nCS = 1;
  for(i=0; i<71; ++i)
  {
    SPDAT = 0xFF;
    while((SPSTA & 0x80) != 0x80);
  }
  
  // Send CMD0
  spi_sdcard_command(0, 0, SDCARD);
  error = spi_sdcard_response(1, response);
  
  // Send CMD8
  if(error == 0)
  {
    spi_sdcard_command(8, 0x000001AA, SDCARD);
    error = spi_sdcard_response(5, response);
    /* Disabled for code size
    if(error == 0)
    {
      if(*response32 != 0x000001AA)
        error = 3;
    }*/
  }
  
  // Send CMD58
  if(error == 0)
  {
    spi_sdcard_command(58, 0, SDCARD);
    error = spi_sdcard_response(5, response);
    /* Disabled for code size
    if(error == 0 && (response[2] & 0x30) != 0x30)
      error = 4;*/
  }
  
  // Send ACMD41
  while(response[0] != 0x00 && error == 0) //until active state
  {
    spi_sdcard_command(55, 0, SDCARD);
    error = spi_sdcard_response(1, response);
    spi_sdcard_command(41, 0x40000000, SDCARD);
    error = spi_sdcard_response(1, response);
  }
  
  // Send CMD58
  if(error == 0)
  {
    spi_sdcard_command(58, 0, SDCARD);
    error = spi_sdcard_response(5, response);
    if(error == 0 && (response[1] & 0x40) != 0x40)
    {
      // Standard capacity card
      spi_sdcard_command(16, 512, SDCARD);
      error = spi_sdcard_response(1, response);
      spi_sdcard_standardCapacity = 1;
    }
  }
  
  // Send CMD9
  if(error == 0)
  {
    spi_sdcard_command(9, 0, SDCARD);
    error = spi_sdcard_block(16, response);
  }
  
  // Send CMD10
  if(error == 0)
  {
    spi_sdcard_command(10, 0, SDCARD);
    error = spi_sdcard_block(16, response);
  }
  
  if(error != 0)
  {
    redLED = 0;
    /*uart_print("SD card error ", 15);
    uart_hex8(error);
    uart_print("\n\r", 2);*/
  }
}

void spi_sdcard_command(uint8 cmd, uint32 arg, bit device)
{
  uint8 checkSum = 0x01;
  
  greenLED = 0;
  
  // Checksum
  if(cmd == 0)
  {
    checkSum = 0x95;
    arg = (0UL);
  }
  else if(cmd == 8)
    checkSum = 0x87;
  else if(cmd == 17 && spi_sdcard_standardCapacity)
    arg *= 512;
  
  cmd &= 0x7F;
  cmd |= 0x40;
  
  nCS = device;
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
  
  if(device == MP3)
    nCS = ~device;
  
  greenLED = 1;
}

uint8 spi_sdcard_response(uint8 numBytes, uint8* buffer)
{
  uint8 in, i;
  uint8* current; 
  uint8 timeout;
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
        timeout = 0;
        SPDAT = 0xFF;
        while((SPSTA & 0x80) != 0x80 && ++timeout != 255);
        *current = SPDAT;
        current++;
        
        if(timeout == 255)
          error = 2;
      }
    }
  }
  else
      error = 1;
    
  SPDAT = 0xFF;
  while((SPSTA & 0x80) != 0x80);
  
  nCS = 1;
  amberLED = 1;
  
  return error;
}

uint8 spi_sdcard_block(uint16 numBytes, uint8* buffer)
{
  uint8 in;
  uint16 i; 
  uint8 timeout;
  uint8 error = 0;
  
  amberLED = 0;
  
  do
  {
    SPDAT = 0xFF;
    while((SPSTA & 0x80) != 0x80);
    in = SPDAT;
  } while(in == 0xFF);
  
  if(in == 0x00)
  {
    
    do
    {
      SPDAT = 0xFF;
      while((SPSTA & 0x80) != 0x80);
      in = SPDAT;
    } while(in == 0xFF);
    
    if(in == 0xFE)
    {
      // Read block
      for(i=0; i<numBytes; ++i)
      {
        timeout = 0;
        SPDAT = 0xFF;
        while((SPSTA & 0x80) != 0x80 && ++timeout != 255);
        buffer[i] = SPDAT;
            
        if(timeout == 255)
          error = 2;
      }
      
      // Read checksum
      for(i=0; i<2; ++i)
      {
        SPDAT = 0xFF;
        while((SPSTA & 0x80) != 0x80 && ++timeout != 255);
        in = SPDAT;
        
        if(timeout == 255)
          error = 2;
      }
    }
    else
      error = 3;
  }
  else
    error = 1;
  
  nCS = 1;
  amberLED = 1;
  
  return error;
}

