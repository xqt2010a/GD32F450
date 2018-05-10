#ifndef __SPEED_H__
#define __SPEED_H__

#include "misc.h"
#include "stdio.h"
#include "SEGGER_RTT.h"
#include "stm32f10x_gpio.h"

void Speed_Init(void);
unsigned int GetLeftSpeed(void); 
unsigned int GetRightSpeed(void);

#endif  /* __SPEED_H__ */