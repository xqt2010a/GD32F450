#include "jx_uart.h"
#include "stdio.h"

void main(void)
{
    uart_init(115200);
    printf("hello world!\r\n");
    while(1);
}