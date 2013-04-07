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
  uint16 idata value;
  uint8 idata* ptr;
  
  ptr = (&value) + 1;
  *ptr = array[offset];
  *(++ptr) = array[--offset];
  
  return value;
}

uint32 read32(uint16 offset, uint8 xdata* array)
{
  uint32 idata value;
  uint8 idata* ptr;
  
  ptr = (&value) + 3;
  *ptr = array[offset];
  *(++ptr) = array[--offset];
  *(++ptr) = array[--offset];
  *(++ptr) = array[--offset];
  
  return value;
}
