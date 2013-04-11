/************************************************
 * filesystem.c
 *
 * Jason Gassel
 ************************************************/
 
#include "filesystem.h" 

uint8 xdata fs_sdBuffer1[512];
uint8 xdata fs_sdBuffer2[512];
uint8 xdata* fs_buffer;

void fs_init()
{
  uint8 error = 0;
  uint32 bootSector;
  
  fs_buffer = &fs_sdBuffer1;
  
  spi_sdcard_command(17, 0);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    if(read8(0x0000, fs_buffer) == 0xEB) // Boot sector
      bootSector = 0;
    else // Master boot record, locate the boot sector
      bootSector = read32(0x01C6, fs_buffer);
    uart_print("Boot Sector ", 12);
    uart_hex32(bootSector);
    uart_print("\n\r", 2);
  }
  
  // Read boot sector
  spi_sdcard_command(17, bootSector);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    //TODO read values from boot sector
  }
  
  if(error != 0)
      redLED = 0;
}

uint8 read8(uint16 offset, uint8 xdata* array)
{
  return array[offset];
}

uint16 read16(uint16 offset, uint8 xdata* array)
{
  uint16 idata value;
  uint8 idata* ptr;
  
  ptr = ((uint8 idata*)&value) + 1;
  *ptr = array[offset];
  *(++ptr) = array[--offset];
  
  return value;
}

uint32 read32(uint16 offset, uint8 xdata* array)
{
  uint32 idata value;
  uint8 idata* ptr;
  
  ptr = ((uint8 idata*)&value) + 3;
  *ptr = array[offset];
  *(++ptr) = array[--offset];
  *(++ptr) = array[--offset];
  *(++ptr) = array[--offset];
  
  return value;
}
