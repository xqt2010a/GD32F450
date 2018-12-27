#ifndef __JX_INT_H__
#define __JX_INT_H__

#include "jx.h"





void IRQ_Init(void);
void IRQ_SetEnable(void);
int IRQ_Register(unsigned int irqnum, void f(void));


#endif  /* __JX_INT_H__ */