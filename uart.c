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

void uart_print(uint8 string[], uint8 length)
{
  uint8 byte;
  
  if(uart_bufferPos != 0)
    return; //already transmitting
  
  // Copy string to buffer so that memory won't be overwritten
  for(byte=0; byte<length; ++byte)
    uart_buffer[byte] = string[byte];
  uart_bufferLength = length;
  
  // Transmit first byte
  SBUF = uart_buffer[0];
  uart_bufferPos = 1;
}

void uart_ISR(void) interrupt 4 using 2
{
  if(TI)
  {
    // Transmitted
    TI = 0;
    
    if(uart_bufferPos < uart_bufferLength)
    {
      SBUF = uart_buffer[uart_bufferPos];
      uart_bufferPos++;
    }
    else
      uart_bufferPos = 0;
  }
  else if(RI)
  {
    // Received
    RI = 0;
  }
}
