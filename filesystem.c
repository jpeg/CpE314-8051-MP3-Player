/************************************************
 * filesystem.c
 *
 * Jason Gassel
 ************************************************/
 
#include "filesystem.h" 

uint8 read8(uint16 offset, uint8 xdata* array)
{
  return array[offset];
}

uint16 read16(uint16 offset, uint8 xdata* array)
{
  uint16 value;
  
  value = array[offset];
  value |= array[++offset] << 8;
  
  return value;
}

uint32 read32(uint16 offset, uint8 xdata* array)
{
  uint32 value;
  
  value = array[offset];
  value |= array[++offset] << 8;
  value |= array[++offset] << 16;
  value |= array[++offset] << 24;
  
  return value;
}
