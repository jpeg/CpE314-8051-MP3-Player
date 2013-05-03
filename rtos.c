/************************************************
 * rtos.c
 *
 * Jason Gassel
 ************************************************/

#include "rtos.h"

static uint8 rtos_numTasks;
static task_type idata rtos_tasks[RTOS_MAX_NUM_TASKS];
static uint16 idata rtos_taskCounts[RTOS_MAX_NUM_TASKS];
static uint16 idata rtos_taskCounters[RTOS_MAX_NUM_TASKS];

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
  AUXR = 0x0D;
  
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
  fs_init();
}

/*void rtos_task(task_type task, uint16 msec)
{
  rtos_tasks[rtos_numTasks] = task;
  if(msec > 0)
    rtos_taskCounts[rtos_numTasks] = msec;
  else
    rtos_taskCounts[rtos_numTasks] = 1;
  rtos_taskCounters[rtos_numTasks] = 1;
  rtos_numTasks++;
}*/

void rtos_spin()
{
  bit spin = 1;
  uint8 idata array[1] = {0x01};
  uint8 error = 0;
  redLED=0;
  do
  {
    error = twi_write(0x43, 1, array); 
  } while(error != 0);redLED=1;greenLED=0;
  do
  {
    error = twi_read(0x43, 1, array);
  } while(error != 0);greenLED=1;
  uart_hex8(array[0]);
  
  while(spin);
  {
  }
}

/*void rtos_tick_ISR(void) interrupt 5 using 3
{
  uint8 currentTask;
  
	TF2 = 0;

  // Run all tasks
	for(currentTask=0; currentTask<rtos_numTasks; ++currentTask)
  {
    if(--rtos_taskCounters[currentTask] == 0)
    {
      rtos_tasks[currentTask]();
      rtos_taskCounters[currentTask] = rtos_taskCounts[currentTask];
    }
  }
}*/
