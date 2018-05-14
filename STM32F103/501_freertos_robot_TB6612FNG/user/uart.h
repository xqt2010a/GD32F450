#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

extern void Uart_Init(void);
extern void Uart_StrSend(uint8_t *pbuf, uint8_t len);

#endif  /* __UART_H__ */