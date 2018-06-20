#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f10x_i2c.h"

#define WORD_ADDR_SIZE    8//16

extern void IIC_Init(void);
extern void IIC_WriteBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len);
extern void IIC_ReadBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len);

#endif  /* __IIC_H__ */