#include "jx_uart.h"
#include "stdio.h"
#include "mmc_boot.h"
#include "system.h"

UART_InitTypeDef uart;

void uart_start_init(void)
{
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.int_mask = 0;
}

void main(void)
{
    uart_start_init();
    uart_init(&uart);
    printf("mmc read write test!\r\n");
    mmc_test();
    while(1);
}