#ifndef __UART_TASK_H__
#define __UART_TASK_H__

#include "uart.h"

#define UART_BUF_LEN    40

extern uint8_t Uart1_Buf_Rx[UART_BUF_LEN];
extern uint8_t Uart1_Sta_Rx;
extern uint8_t Uart1_Index_Rx;

extern uint8_t Uart2_Buf_Rx[UART_BUF_LEN];
extern uint8_t Uart2_Sta_Rx;
extern uint8_t Uart2_Index_Rx;

extern void vUart1_Task(void *p);
extern void vUart1_Report_Task(void *p);

extern void vUart2_Task(void *p);
extern void vUart2_Report_Task(void *p);

#endif  /* __UART_TASK_H__ */