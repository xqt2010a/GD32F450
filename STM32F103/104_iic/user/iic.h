#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f10x_i2c.h"

#define WORD_ADDR_SIZE    8//16

#define SDA_PIN     6       //PB6 SDA   PB7 SCL
//#define SDA_IN()    {GPIOB->CRL &= (0xFFFFFFFF&(0<<(SDA_PIN*4))); GPIOB->CRL |= 8<<(SDA_PIN*4);}
//#define SDA_OUT()   {GPIOB->CRL &= (0xFFFFFFFF&(0<<(SDA_PIN*4))); GPIOB->CRL |= 3<<(SDA_PIN*4);}

#define IIC_SCL(x)      GPIO_WriteBit(GPIOB, GPIO_Pin_7, x);
#define IIC_SDA(x)      GPIO_WriteBit(GPIOB, GPIO_Pin_6, x);
#define READ_SDA        GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)

extern void delay_us(uint32_t t);
extern void IIC_Start(void);
extern void IIC_Stop(void);
extern uint8_t IIC_WaitAck(void);
extern void IIC_Ack(void);
extern void IIC_NAck(void);
extern void IIC_WriteByte(uint8_t data);
extern uint8_t IIC_ReadByte(uint8_t ack);

extern void IIC_Init(void);
extern void IIC_WriteBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len);
extern void IIC_ReadBytes(uint8_t device_addr, uint16_t addr, uint8_t *data, uint16_t len);

#endif  /* __IIC_H__ */