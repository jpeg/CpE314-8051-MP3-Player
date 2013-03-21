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
  while(spin)
  {
    // Sleep until next interrupt
    PCON |= 0x01;
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
