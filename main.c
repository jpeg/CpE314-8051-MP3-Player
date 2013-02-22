/************************************************
 * main.c
 *
 * Jason Gassel
 ************************************************/

#include "main.h"
#include "ports.h"

void main(void)
{
    bit flashState = 0;
    uint16 count;
    
	while(1)
	{
        redLED = flashState;
        yellowLED = flashState;
        amberLED = flashState;
        greenLED = flashState;
        flashState = ~flashState;
        for(count=0; count<25000; count++); //delay
	}
}
