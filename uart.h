/************************************************
 * uart.h
 *
 * Jason Gassel
 ************************************************/

#ifndef UART_H
#define UART_H

#include "main.h"
#include "ports.h"

void uart_init(void);

void uart_print(uint8* string, uint8 length);

void uart_hex8(uint8 c);

//void uart_hex32(uint32 byte);

//void uart_dump(uint8* block, uint16 bytes);

#endif //UART_H
