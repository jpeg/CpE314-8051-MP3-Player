/************************************************
 * uart.h
 *
 * Jason Gassel
 ************************************************/

#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include "main.h"

#define UART_BUFFER_SIZE (33)
static idata uint8 uart_buffer[UART_BUFFER_SIZE];
static uint8 uart_bufferLength;
static uint8 uart_bufferPos = 0;

void uart_init(void);

void uart_print(uint8* string, uint8 length);

void uart_ISR(void);

#endif //UART_H
