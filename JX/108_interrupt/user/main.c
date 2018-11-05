#include "jx_uart.h"
#include "jx_int.h"
#include "system.h"
#include "stdio.h"



void main(void)
{
    uint32_t sysclk;
    IRQ_Init();
    IRQ_Register(110, uart_irq);
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\n");
    while(1);
}