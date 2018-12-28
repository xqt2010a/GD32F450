#ifndef __JX_INT_H__
#define __JX_INT_H__

#include "gic.h"

void int_init(void);
void int_enable(void);
void int_register(unsigned int irqnum, void f(void));


#endif  /* __JX_INT_H__ */