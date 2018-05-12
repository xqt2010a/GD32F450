#ifndef __UART_TASK_H__
#define __UART_TASK_H__

#include "uart.h"

#define UART_BUF_LEN    40

extern uint8_t Uart_Buf_Rx[UART_BUF_LEN];
extern uint8_t Uart_Sta_Rx;
extern uint8_t Uart_Index_Rx;

extern void vUart_Task(void *p);
extern void vReport_Task(void *p);

#endif  /* __UART_TASK_H__ */