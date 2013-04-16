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
  uint32 currentDirectory = 2;
  uint32 result;
  uint8 choice;
  uint8 byte;
  bit add;
  bit input;
  uint8 error = 0;
  
  while(spin);
  {
    // Display directory entries list
    fs_listDirectoryEntries(currentDirectory);
    
    uart_print("Choice: ", 8);
    
    // Input block hex number
    choice = 0;
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
      {
        choice = choice >> 4;
      }
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
        choice = choice << 4;
        choice |= choice;
      }
    }
    
    // Display file or set new directory
    result = fs_findChoice(currentDirectory, choice);
    if(result != 0x00000000) //directory was chosen
      currentDirectory = result;
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
