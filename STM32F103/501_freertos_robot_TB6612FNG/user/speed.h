#ifndef __SPEED_H__
#define __SPEED_H__

#include "misc.h"
#include "stdio.h"
#include "SEGGER_RTT.h"
#include "stm32f10x_gpio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "robot_protocol.h"


#define DEBUG_COUNT     0   //�򿪱��������Ա���������


extern void Speed_Init(void);
extern void vTask_Speed(void *p);

#endif  /* __SPEED_H__ */