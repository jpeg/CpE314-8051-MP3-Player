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
    ET2 = 1;
    TR2 = 1;
    
    EA = 1;
    
    rtos_numTasks = 0;
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

void rtos_spin()
{
    bit spin = 1;
    while(spin)
    {
        //TODO Print
        
        // Sleep until next interrupt
        PCON |= 0x01;
    }
}

void rtos_tick_ISR(void) interrupt 5 using 3
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
}
