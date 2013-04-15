/************************************************
 * filesystem.c
 *
 * Jason Gassel
 ************************************************/
 
#include "filesystem.h" 

uint8 xdata fs_sdBuffer1[512];
uint8 xdata fs_sdBuffer2[512];
static uint8 xdata* fs_buffer;

enum FATtype { FAT16 = 1, FAT32 = 2 };
static uint8 idata fs_FAToffset;
static uint16 idata fs_FATreservedSectorCount;
static uint32 idata fs_FATstartSector;
static uint32 idata fs_FATfirstDataSector;
static uint32 idata fs_FATsectorsPerCluster;

void fs_init()
{
  uint8 error = 0;
  uint32 relativeSectors;
  uint16 rootEntCnt;
  uint16 rootDirSectors;
  
  fs_buffer = &fs_sdBuffer1;
  
  spi_sdcard_command(17, 0);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    if(read8(0x0000, fs_buffer) == 0xEB) // Boot sector
      relativeSectors = 0;
    else // Master boot record, locate the boot sector
      relativeSectors = read32(0x01C6, fs_buffer);
    uart_print("Boot Sector ", 12);
    uart_hex32(relativeSectors);
    uart_print("\n\r", 2);
  }
  
  // Read boot sector
  fs_swapBuffer();
  spi_sdcard_command(17, relativeSectors);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    if(read8(0x0016, fs_buffer) == 0)
      fs_FAToffset = FAT32;
    else
      fs_FAToffset = FAT16;
    
    fs_FATreservedSectorCount = read16(0x000E, fs_buffer);
    rootEntCnt = read16(0x0011, fs_buffer);
    rootDirSectors = ((rootEntCnt*32) + (512-1)) / 512;
    
    fs_FATstartSector = relativeSectors + fs_FATreservedSectorCount;
    
    if(fs_FAToffset == FAT32)
    {
      fs_FATfirstDataSector = read32(0x002C, fs_buffer);
    }
    else // FAT16
    {
      fs_FATfirstDataSector = fs_FATreservedSectorCount + 2 << fs_FAToffset + rootDirSectors;//relative sectors?
    }
    
    fs_FATsectorsPerCluster = read8(0x000D, fs_buffer);
    
    
    // Assume bytes per sector = 512 for code size
    if(read16(0x000B, fs_buffer))
      error = 11;
    
    //DEBUG
    uart_dump(fs_buffer, 512);
  }
  
  if(error != 0)
      redLED = 0;
}


uint32 fs_FATentry(uint32 cluster)
{
  uint32 FATsectorNumber;
  uint32 FATentryOffset;
  
  FATsectorNumber = fs_FATreservedSectorCount + (cluster << fs_FAToffset) / 512;
  FATentryOffset = (cluster << fs_FAToffset) % 512;
  
  fs_swapBuffer();
  spi_sdcard_command(17, FATsectorNumber);
  spi_sdcard_block(512, fs_buffer);
  
  return read32(FATentryOffset, fs_buffer) & (fs_FAToffset == FAT32 ? 0xFFFF : 0x0FFFFFFF);
}

uint32 fs_selectDirectoryEntry(uint32 startCluster)
{
}

void fs_longFilename(uint32 cluster)
{
}

void fs_swapBuffer()
{
  if(fs_buffer == &fs_sdBuffer1)
    fs_buffer = &fs_sdBuffer2;
  else
    fs_buffer = &fs_sdBuffer1;
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
  *(--ptr) = array[++offset];
  
  return value;
}

uint32 read32(uint16 offset, uint8 xdata* array)
{
  uint32 idata value;
  uint8 idata* ptr;
  
  ptr = ((uint8 idata*)&value) + 3;
  *ptr = array[offset];
  *(--ptr) = array[++offset];
  *(--ptr) = array[++offset];
  *(--ptr) = array[++offset];
  
  return value;
}
