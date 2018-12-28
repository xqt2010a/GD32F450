#include "jx_uart.h"
#include "jx_int.h"
#include "system.h"
#include "stdio.h"

UART_InitTypeDef uart;
int uart_rx_count=0;
uint8_t uart_buf_rx[64];


void uart_struct_init(void)
{
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.int_mask = 5;  //enable rx interrupt
}

void uart_irq(void)
{
    uart_buf_rx[uart_rx_count++] = dwc_uart_rx(&uart);
}


void main(void)
{
    int i;
    
    uart_struct_init();
    uart_init(&uart);
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    
    int_init();
    int_register(110, uart_irq);
    int_enable();
    
    while(1){
        i = uart_rx_count;
        if(i>0){
            while(uart_rx_count--){
                dwc_uart_tx(&uart, uart_buf_rx[i-uart_rx_count-1]);
            }
        }
    }
}