/************************************************
 * uart.c
 *
 * Jason Gassel
 ************************************************/

#include "uart.h"

void uart_init(void)
{
  PCON = 0x80;
  SCON = 0x40;
  BDRCON &= 0xEE;
  BDRCON |= 0x0E;
  BRL = 256 - 2*OSC_FREQ / (32*BAUDRATE);
  BDRCON |= 0x10;
  
  IPH0 &= 0xEF; // Priority 0
  IPL0 &= 0xEF;
  ES = 1;
  
  EA = 1;
}

void uart_print(uint8* string, uint8 length)
{
  uint8 i;
  
  for(i=0; i<length; ++i)
  {
    SBUF = string[i];
    while(!TI);
    TI = 0;
  }
}

void uart_hex8(uint8 c)
{
  uint8 result[3];
  uint8 i;
  
  result[0] = c >> 4;
  result[1] = c & 0x0F;
  
  for(i=0; i<2; ++i)
  {
    if(result[i] < 0x0A)
      result[i] += 48;
    else
      result[i] += 55;
  }
  
  result[2] = ' ';
  
  uart_print(result, 3);
}

void uart_hex32(uint32 byte)
{
  uint8 result[9];
  uint8 i;
  
  result[0] = (uint8)(byte >> 28) & 0x0F;
  result[1] = (uint8)(byte >> 24) & 0x0F;
  result[2] = (uint8)(byte >> 20) & 0x0F;
  result[3] = (uint8)(byte >> 16) & 0x0F;
  result[4] = (uint8)(byte >> 12) & 0x0F;
  result[5] = (uint8)(byte >> 8) & 0x0F;
  result[6] = (uint8)(byte >> 4) & 0x0F;
  result[7] = (uint8)byte & 0x0F;
  
  for(i=0; i<8; ++i)
  {
    if(result[i] < 0x0A)
      result[i] += 48;
    else
      result[i] += 55;
  }
  
  result[8] = ' ';
  
  uart_print(result, 9);
}

void uart_dump(uint8* block, uint16 bytes)
{
  uint16 byte;
  uint16 position = 0;
  uint8 i;
  
  uart_print("ADDRESS  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ASCII\n\r", 64);
  
  for(byte=0; byte<bytes; byte+=16)
  {
    uart_hex32((uint32)byte);
    
    for(i=0; i<16; ++i)
    {
      uart_hex8(block[position]);
      if(block[position] < 0x20 || block[position] > 0x7E)
        block[position] = '.';
      position++;
    }
    
    uart_print(&block[byte], 16);
    uart_print("\n\r", 2);
  }
  
  uart_print("\n\r", 2);
}
