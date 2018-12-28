#include "jx_uart.h"
#include "system.h"

void uart_init(UART_InitTypeDef *uart)
{
    uart->clk = get_sysclk()/4;
    dwc_uart_init(uart);
}