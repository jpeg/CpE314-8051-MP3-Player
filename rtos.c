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
  uint32 idata fileCluster;
  uint8 i;
  uint8 error = 0;
  
  // Arrays from config.asm
  extern code uint8* CONFIG;
  extern code uint8* CONFIG2;
  uint8 code* configPtr;
  uint8 idata temp[2];
  
  // Reset
  mp3_reset = 0;
  for(i=0; i<80; i++);
  mp3_reset = 1;
  
  // Test TWI
  do
  {
    error = twi_write(0x43, 1, array); 
  } while(error != 0);
  do
  {
    error = twi_read(0x43, 1, array);
  } while(error != 0);
  if(array[0] != 0xAC)
  	redLED = 0;
  
  // Send STAO13 config file
  configPtr = CONFIG;
  while(*configPtr != 0xFF)
  {
    temp[0] = *configPtr;
    configPtr++;
    temp[1] = *configPtr;
    configPtr++;
    error = twi_write(0x43, 2, temp);
  }
  
  // Pause
  for(i=0; i<80; ++i);
  
  // Send second STAO13 config
  configPtr = CONFIG2;
  while(*configPtr != 0xFF)
  {
    temp[0] = *configPtr;
    configPtr++;
    temp[1] = *configPtr;
    configPtr++;
    error = twi_write(0x43, 2, temp);
  }
  
  for(i=0; i<80; ++i);
  
  fileCluster = fs_findMP3(2);
  if(fileCluster == 0)
  {
    amberLED = 0;
    greenLED = 0;
    uart_print("ERROR: No MP3 in root\n\r", 23);
  }
  else
  {
    uart_print("Found MP3\n\r", 11);
    
    // Load first sector
    fs_loadSector(fileCluster, 0);
    for(i=1; i<64; ++i)
    {
      // Send data to MP3 decoder
      spi_mp3_data(512, fs_currentBuffer());
      
      // Load data from SD card
      fs_swapBuffer();
      fs_loadSector(fileCluster, i);
      
      // Wait for DATA_REQ
      while(mp3_data_req == 0);
      yellowLED = ~yellowLED;
    }
    yellowLED = 1;
  }
  
  while(spin)
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
