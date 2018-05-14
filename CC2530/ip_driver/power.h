#ifndef __POWER_H__
#define __POWER_H__

#include "ioCC2530.h"

#define XOSC       0      //Ê¯Ó¢¾§Õñ
#define RCOSC      1      //RCÄÚ²¿¾§Õñ

extern void PWR_GpioInit(void);
extern void PWR_Mode(unsigned char mode);
extern void PWR_MainClock(unsigned char source);
extern void PWR_LowClock(unsigned char source);

#endif  /* __POWER_H__ */