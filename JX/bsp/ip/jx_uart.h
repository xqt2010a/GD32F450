#ifndef __JX_UART_H__
#define __JX_UART_H__

#include "jx.h"
#include "dwc_uart.h"

typedef enum{
    UART0  = 0,
    UART1  = 1,
    UART2  = 2,
    UART3  = 3
}UART_n;

/**
  * @brief  UART Init structure definition
  */
  
typedef struct
{
    UART_n ch;
    uint8_t IntEnable;    /* 0:disable 1:enable */
    uint32_t clk;
	uint32_t bandrate;
} UART_InitTypeDef;

void uart_init(UART_InitTypeDef *uart);
void uart_tx(UART_InitTypeDef *uart, uint16_t data);
uint8_t uart_rx(UART_InitTypeDef *uart);

#endif /* __JX_UART_H__ */