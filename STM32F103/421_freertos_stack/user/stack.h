#ifndef __STACK_H__
#define __STACK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "SEGGER_RTT.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

extern void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
extern void vTask_PRT(void *pvParameters);

#endif  /* __STACK_H__ */