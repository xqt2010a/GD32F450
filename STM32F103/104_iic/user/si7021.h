#ifndef __SI7021_H__
#define __SI7021_H__

#include "iic.h"

extern void SI7021_Init(void);
extern uint32_t Read_T(void);
extern uint32_t Read_RH(void);

#endif  /* __SI7021_H__ */