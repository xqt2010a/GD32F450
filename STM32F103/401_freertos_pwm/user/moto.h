#ifndef __MOTO_H__
#define __MOTO_H__

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"

extern void Moto_Init(void);
extern void vTask_Moto(void *p);

#endif  /* __MOTO_H__ */