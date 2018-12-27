#ifndef __JX_UART_H__
#define __JX_UART_H__

#include "dwc_uart.h"

typedef enum{
    UART0  = 0,
    UART1  = 1,
    UART2  = 2,
    UART3  = 3
}UART_n;

void uart_init(UART_InitTypeDef *uart);

#endif /* __JX_UART_H__ */