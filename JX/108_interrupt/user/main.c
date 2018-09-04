#include "jx_uart.h"
#include "jx_int.h"
#include "system.h"
#include "stdio.h"

void uart_irq(void)
{

}

void main(void)
{ 
    SystemInit();
    IRQ_Init();
    IRQ_Register(1, uart_irq);
    uart_init(115200);
    printf("hello world!\r\n");
    while(1);
}