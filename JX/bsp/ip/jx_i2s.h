#ifndef __JX_I2S_H__
#define __JX_I2S_H__

#include "dwc_i2s.h"

typedef enum
{
    I2S0    = 0,
    I2S1    = 1,
    I2S2    = 2
}I2Sn;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
void I2S_Init(I2S_InitTypeDef* i2s);

#endif  /* __JX_I2S_H__ */