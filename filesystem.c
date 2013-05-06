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
static uint32 idata fs_FATrelativeSectors;
static uint16 idata fs_FATreservedSectorCount;
static uint32 idata fs_FATstartSector;
static uint8 idata fs_FATnum;
static uint32 idata fs_FATsize;

// Define extern variables
uint32 idata fs_FATfirstDataSector;
uint32 idata fs_FATsectorsPerCluster;

void fs_init()
{
  uint8 error = 0;
  uint16 rootEntCnt;
  uint16 rootDirSectors;
  
  fs_buffer = &fs_sdBuffer1;
  
  spi_sdcard_command(17, 0, SDCARD);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    if(read8(0x0000, fs_buffer) == 0xEB) // Boot sector
      fs_FATrelativeSectors = 0;
    else // Master boot record, locate the boot sector
      fs_FATrelativeSectors = read32(0x01C6, fs_buffer);
    //uart_print("Boot Sector ", 12);
    uart_hex32(fs_FATrelativeSectors);
    uart_print("\n\r", 2);
  }
  
  // Read boot sector
  fs_swapBuffer();
  spi_sdcard_command(17, fs_FATrelativeSectors, SDCARD);
  error = spi_sdcard_block(512, fs_buffer);
  if(error == 0)
  {
    //uart_print("File System ", 12);
    if(read8(0x0016, fs_buffer) == 0)
    {
      fs_FAToffset = FAT32;
      fs_FATsize = read32(0x0024, fs_buffer);
      uart_print("FAT32", 5);
    }
    else
    {
      fs_FAToffset = FAT16;
      fs_FATsize = read16(0x0016, fs_buffer);
      uart_print("FAT16", 5);
    }
    uart_print("\n\r", 2);
    
    fs_FATreservedSectorCount = read16(0x000E, fs_buffer);
    rootEntCnt = read16(0x0011, fs_buffer);
    rootDirSectors = ((rootEntCnt*32) + (512-1)) / 512;
    
    fs_FATstartSector = fs_FATrelativeSectors + fs_FATreservedSectorCount;
    fs_FATnum = read8(0x0010, fs_buffer);
      
    
    fs_FATfirstDataSector = fs_FATreservedSectorCount + fs_FATnum * fs_FATsize 
                            + rootDirSectors + fs_FATrelativeSectors;

    
    fs_FATsectorsPerCluster = read8(0x000D, fs_buffer);
    
    
    // Assume bytes per sector = 512 for code size
    if(read16(0x000B, fs_buffer) != 512)
      error = 0x0A;
  }
  
  if(error != 0)
  {
    redLED = 0;
    /*uart_print("FAT Error ", 15);
    uart_hex8(error);
    uart_print("\n\r", 2);*/
  }
  
  uart_print("\n\r", 2);
}


uint32 fs_FATentry(const uint32 cluster)
{
  uint32 FATsectorNumber;
  uint32 FATentryOffset;
  
  FATsectorNumber = fs_FATreservedSectorCount + (cluster << fs_FAToffset) / 512 + fs_FATrelativeSectors;
  FATentryOffset = (cluster << fs_FAToffset) % 512;
  
  spi_sdcard_command(17, FATsectorNumber, SDCARD);
  spi_sdcard_block(512, fs_buffer);
  
  return read32(FATentryOffset, fs_buffer) & (fs_FAToffset == FAT32 ? 0xFFFF : 0x0FFFFFFF);
}

/*void fs_listDirectoryEntries(const uint32 startCluster)
{
  uint32 idata cluster = startCluster;
  uint8 idata relativeSector = 0;
  uint16 idata relativeEntry = 0;
  bit longFilename = 0;
  uint8 idata entryCount = 0;
  uint8 idata byte;
  uint8 idata i;
   
  while(cluster != 0x0FFFFFFF && cluster != 0x0000FFFF)
  {
    while(relativeSector < fs_FATsectorsPerCluster)
    {
      // Load sector
      spi_sdcard_command(17, (cluster-2)*fs_FATsectorsPerCluster + relativeSector + fs_FATfirstDataSector);
      spi_sdcard_block(512, fs_buffer);
      
      while(relativeEntry < 512)
      {
        byte = read8(relativeEntry, fs_buffer);
        if(byte == 0x00)
          break;
        if(byte == 0xE0)
          continue;
        
        switch(read8(relativeEntry + 11, fs_buffer))
        {
        case 0x0F: // Long filename
        case 0x1F:
        case 0x2F:
          longFilename = 1;
          // Ignore long filenames for now
          break;
          
        case 0x00: // File
        case 0x20: // Archive
          uart_hex8(entryCount++);
          uart_print(")     ", 6);
          for(i=0; i<8 && *(fs_buffer+relativeEntry+i) != ' '; ++i)
            uart_print(fs_buffer + relativeEntry + i, 1);
          uart_print(".", 1);
          uart_print(fs_buffer + relativeEntry + 8, 3);
          uart_print("\n\r", 2);
          longFilename = 0;
          break;
          
        case 0x10: // Directory
          uart_hex8(entryCount++);
          uart_print(") DIR ", 6);
          uart_print(fs_buffer + relativeEntry, 11);
          uart_print("\n\r", 2);
          break;
          
        default:
          break;
        }
        
        relativeEntry += 32;
      }
      if(byte == 0x00)
        break;
      
      relativeSector++;
      relativeEntry = 0;
    }
    
    cluster = fs_FATentry(cluster);
    relativeSector = 0;
  }
}

uint32 fs_findChoice(const uint32 startCluster, const uint8 choice)
{
  uint32 idata cluster = startCluster;
  uint8 idata relativeSector = 0;
  uint16 idata relativeEntry = 0;
  uint8 idata entryCount = 0;
  uint8 idata byte;
  uint32 idata returnVal = 0x00000000;
  uint32 idata entryCluster;
  bit notDone = 1;
   
  while(cluster != 0x0FFFFFFF && cluster != 0x0000FFFF && notDone)
  {
    while(relativeSector < fs_FATsectorsPerCluster && notDone)
    {
      // Load sector
      spi_sdcard_command(17, (cluster-2)*fs_FATsectorsPerCluster + relativeSector + fs_FATfirstDataSector);
      spi_sdcard_block(512, fs_buffer);
      
      while(relativeEntry < 512 && notDone)
      {
        byte = read8(relativeEntry, fs_buffer);
        if(byte == 0x00)
          break;
        if(byte == 0xE0)
          continue;
        
        entryCluster = read16(0x1A, fs_buffer);
        if(fs_FAToffset == FAT32)
          entryCluster |= read16(0x14, fs_buffer) << 8;
        
        byte = read8(relativeEntry + 11, fs_buffer);
        if(byte == 0x00 || byte == 0x20) // File or archive
        {
          entryCount++;
          if(entryCount == choice)
          {
            fs_printFile(entryCluster);
            notDone = 0;
          }
        }
        else if(byte == 0x10) // Directory
        {
          entryCount++;
          if(entryCount == choice)
          {
            returnVal = entryCluster;
            notDone = 0;
          }
        }
        
        relativeEntry += 32;
      }
      if(byte == 0x00)
        break;
      
      relativeSector++;
      relativeEntry = 0;
    }
    
    cluster = fs_FATentry(cluster);
    relativeSector = 0;
  }
  
  return returnVal;
}

void fs_printFile(uint32 cluster)
{
  uint8 idata relativeSector = 0;
  bit notDone = 1;
  uint8 byte;
   
  while(cluster != 0x0FFFFFFF && cluster != 0x0000FFFF && notDone)
  {
    while(relativeSector < fs_FATsectorsPerCluster && notDone)
    {
      // Load sector
      spi_sdcard_command(17, (cluster-2)*fs_FATsectorsPerCluster + relativeSector + fs_FATfirstDataSector);
      spi_sdcard_block(512, fs_buffer);
      
      //uart_print("Cluster ", 8);
      //uart_hex32(cluster);
      uart_print(" Sector ", 8);
      uart_hex32((cluster-2)*fs_FATsectorsPerCluster + relativeSector + fs_FATfirstDataSector);
      uart_print("\n\r", 2);
      
      uart_dump(fs_buffer, 512);
      
      uart_print("Press 'q' to quit, any other key to continue", 44);
      while(!RI);
      RI = 0;
      byte = SBUF;
      uart_print(&byte, 1);
      uart_print("\n\r\n\r", 4);
        
      if(byte == 'q')
        notDone = 0;

      relativeSector++;
    }
    
    cluster = fs_FATentry(cluster);
    relativeSector = 0;
  }
}*/

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
