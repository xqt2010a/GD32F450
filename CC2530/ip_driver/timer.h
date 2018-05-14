#ifndef __TIMER_H__
#define __TIMER_H__

#include "ioCC2530.h"

extern void Timer1_Init(unsigned char value);
extern void Timer1_Disbale(void);
extern void Timer1_Enable(void);

extern void TIMER_SleepInit(void);
extern void TIMER_SleepTime(unsigned int sec);

#endif  /* __TIMER_H__ */