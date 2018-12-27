#include "jx_uart.h"
#include "stdio.h"
#include "system.h"

UART_InitTypeDef uart;

void main(void)
{
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.int_mask = 0;
    uart_init(&uart);
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    while(1);
}