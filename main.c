/************************************************
 * main.c
 *
 * Jason Gassel
 ************************************************/

#include "main.h"
#include "ports.h"
#include "rtos.h"

// Tasks
void task_readSwitches(void);
void task_dumpString(void);

// Task variables
static enum SwitchStates { WAIT_PRESS, PRESS_DETECTED, PRESSED, WAIT_RELEASE, 
               RELEASE_DETECTED };
static uint8 switchStates[4] = { WAIT_PRESS, WAIT_PRESS, WAIT_PRESS, WAIT_PRESS };

void main(void)
{
  // X2 Mode
  CKCON0 = 0x01;
  CKCON1 = 0x00;
  
  // Initialize
  rtos_init();
  rtos_task(&task_readSwitches, 1);
  rtos_task(&task_dumpString, 2000);
  
  // Run
	rtos_spin();
}

void task_readSwitches(void)
{
  // Switch1 debounce
  switch(switchStates[0])
  {
  case WAIT_PRESS:
    if(switch1 == 0)
      switchStates[0] = PRESS_DETECTED;
    break;
  case PRESS_DETECTED:
    if(switch1 == 0)
      switchStates[0] = PRESSED;
    else
      switchStates[0] = WAIT_PRESS;
    break;
  case PRESSED:
    // Do something
    switchStates[0] = WAIT_RELEASE;
    break;
  case WAIT_RELEASE:
    if(switch1 == 1)
      switchStates[0] = RELEASE_DETECTED;
    break;
  case RELEASE_DETECTED:
    if(switch1 == 1)
      switchStates[0] = WAIT_PRESS;
    else
      switchStates[0] = WAIT_RELEASE;
    break;
  default:
    switchStates[0] = WAIT_PRESS;
    break;
  }
  
  // Switch2 debounce
  switch(switchStates[1])
  {
  case WAIT_PRESS:
    if(switch2 == 0)
      switchStates[1] = PRESS_DETECTED;
    break;
  case PRESS_DETECTED:
    if(switch2 == 0)
      switchStates[1] = PRESSED;
    else
      switchStates[1] = WAIT_PRESS;
    break;
  case PRESSED:
    // Do something
    switchStates[1] = WAIT_RELEASE;
    break;
  case WAIT_RELEASE:
    if(switch2 == 1)
      switchStates[1] = RELEASE_DETECTED;
    break;
  case RELEASE_DETECTED:
    if(switch2 == 1)
      switchStates[1] = WAIT_PRESS;
    else
      switchStates[1] = WAIT_RELEASE;
    break;
  default:
    switchStates[1] = WAIT_PRESS;
    break;
  }
  
  // Switch3 debounce
  switch(switchStates[2])
  {
  case WAIT_PRESS:
    if(switch3 == 0)
      switchStates[2] = PRESS_DETECTED;
    break;
  case PRESS_DETECTED:
    if(switch3 == 0)
      switchStates[2] = PRESSED;
    else
      switchStates[2] = WAIT_PRESS;
    break;
  case PRESSED:
    // Do something
    switchStates[2] = WAIT_RELEASE;
    break;
  case WAIT_RELEASE:
    if(switch3 == 1)
      switchStates[2] = RELEASE_DETECTED;
    break;
  case RELEASE_DETECTED:
    if(switch3 == 1)
      switchStates[2] = WAIT_PRESS;
    else
      switchStates[2] = WAIT_RELEASE;
    break;
  default:
    switchStates[2] = WAIT_PRESS;
    break;
  }
  
  // Switch4 debounce
  switch(switchStates[3])
  {
  case WAIT_PRESS:
    if(switch4 == 0)
      switchStates[3] = PRESS_DETECTED;
    break;
  case PRESS_DETECTED:
    if(switch4 == 0)
      switchStates[3] = PRESSED;
    else
      switchStates[3] = WAIT_PRESS;
    break;
  case PRESSED:
    // Do something
    switchStates[3] = WAIT_RELEASE;
    break;
  case WAIT_RELEASE:
    if(switch4 == 1)
      switchStates[3] = RELEASE_DETECTED;
    break;
  case RELEASE_DETECTED:
    if(switch4 == 1)
      switchStates[3] = WAIT_PRESS;
    else
      switchStates[3] = WAIT_RELEASE;
    break;
  default:
    switchStates[3] = WAIT_PRESS;
    break;
  }
}

void task_dumpString(void)
{
  uint8 string[32] = "Testing! Works it does IT DOES\n\r";
  yellowLED = 0;
  uart_dump(string, 32);
  yellowLED = 1;
}
