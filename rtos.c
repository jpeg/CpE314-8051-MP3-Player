/************************************************
 * rtos.c
 *
 * Jason Gassel
 ************************************************/

#include "rtos.h"

void rtos_init(void)
{
  uint16 reload;
  uint8 reloadHigh;
  uint8 reloadLow;
  
	// Tick rate (1 ms)
  reload = (uint16)(65536UL - (OSC_FREQ/1000) / (uint32)OSC_PER_INST);
  reloadHigh = (uint8)(reload / 256);
  reloadLow = (uint8)(reload % 256);
  
  // X2 Mode
  CKCON0 = 0x01;
  CKCON1 = 0x00;
  
  // XRAM Size
  AUXR |= 0x0C;
  
	// Init timer2
  T2CON = 0x04;
  TH2 = reloadHigh;
  RCAP2H = reloadHigh;
  TL2 = reloadLow;
  RCAP2L = reloadLow;
  
  IPH0 &= 0xDF; // Priority 1
  IPL0 |= 0x20;
  ET2 = 1;
  TR2 = 1;
  
  EA = 1;
  
  rtos_numTasks = 0;
  
  uart_init();
  spi_sdcard_init();
}

void rtos_task(task_type task, uint16 msec)
{
  rtos_tasks[rtos_numTasks] = task;
  if(msec > 0)
    rtos_taskCounts[rtos_numTasks] = msec;
  else
    rtos_taskCounts[rtos_numTasks] = 1;
  rtos_taskCounters[rtos_numTasks] = 1;
  rtos_numTasks++;
}

/* Disabled to reduce mem usage
void rtos_rate(task_type task, uint16 msec)
{
  uint8 currentTask;
  
  for(currentTask=0; currentTask<rtos_numTasks; ++currentTask)
  {
    if(rtos_tasks[currentTask] == task)
    {
      rtos_taskCounts[currentTask] = msec;
      if(rtos_taskCounters[currentTask] > rtos_taskCounts[currentTask])
        rtos_taskCounters[currentTask] = msec;
      break;
    }
  }
}

uint16 rtos_getRate(task_type task)
{
  uint8 currentTask;
  
  for(currentTask=0; currentTask<rtos_numTasks; ++currentTask)
  {
    if(rtos_tasks[currentTask] == task)
      return rtos_taskCounts[currentTask];
  }
  
  return 0;
}*/

void rtos_spin()
{
  bit spin = 1;
  uint32 block;
  uint8 byte;
  bit add;
  bit input;
  bit currentXRam = 1;
  
  uint16 j;
  for(j=0; j<512; ++j)
  {
    rtos_sdBuffer[0][j] = (uint8)j;
    rtos_sdBuffer[1][j] = (uint8)(511 - j);
  }
  
  while(spin)
  {
    uart_print("Enter block hex: ", 17);
    
    // Input block hex number
    block = 0;
    input = 1;
    while(input)
    {
      add = 0;
      while(!RI);
      RI = 0;
      byte = SBUF;
      
      uart_print(&byte, 1);
      
      if(byte == '\r')
      {
        input = 0;
        uart_print("\n", 1);
      }
      else if(byte == 0x08) //backspace
        block = block >> 4;
      else if(byte >= 0x30 && byte <= 0x39) //number
      {
        byte &= 0x0F;
        add = 1;
      }
      else if(byte >= 0x41 && byte <= 0x46) //uppercase hex letter
      {
        byte -= 55;
        add = 1;
      }
      else if(byte >= 0x61 && byte <= 0x66) //lowercase hex letter
      {
        byte -= 87;
        add = 1;
      }
      
      if(add)
      {
        block = block << 4;
        block |= (uint32)byte;
      }
    }
    
    
    uart_hex32(block);uart_print("\n\r", 2);
    // Dump block
    currentXRam = ~currentXRam;
    //TODO copy block from SD card
    uart_dump(rtos_sdBuffer[currentXRam], 512);
  }
}

void rtos_tick_ISR(void) interrupt 5 using 3
{
  uint8 currentTask;
  
	TF2 = 0;
  redLED = 0;

  // Run all tasks
	for(currentTask=0; currentTask<rtos_numTasks; ++currentTask)
  {
    if(--rtos_taskCounters[currentTask] == 0)
    {
      rtos_tasks[currentTask]();
      rtos_taskCounters[currentTask] = rtos_taskCounts[currentTask];
    }
  }
  
  redLED = 1;
}
