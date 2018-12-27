#include "jx.h"
#include "jx_uart.h"
#include "stdio.h"
#include "system.h"

#define UART_CLK  83300000//50000000//83300000//11000000  //22M
#define UART_RX_LEN     64

UART_InitTypeDef uart;

uint8_t uart_buf_rx[UART_RX_LEN]={0};
int uart_rx_count = 0;

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void uart_irq(void)
{
    uart_buf_rx[uart_rx_count++] = uart_rx(&uart);
}

void smu_init_m(void)
{
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08104) = 0xFFFFFFFF;
    JX_WR4(0x0190d100) = 0xFFFFFFFF;
    JX_WR4(0x0190d104) = 0xFFFFFFFF;
    JX_WR4(0x0190d108) = 0xFFFFFFFF;
    JX_WR4(0x0190d10c) = 0xFFFFFFFF;
    JX_WR4(0x0190d110) = 0xFFFFFFFF;
    JX_WR4(0x0190d114) = 0xFFFFFFFF;
    JX_WR4(0x0190d120) = 0xFFFFFFFF;
    JX_WR4(0x0190d124) = 0xFFFFFFFF;
    
	/* Delay for somewhile to wait reset de-assertion to be stable. */
	udelay(10000);
}

void main(void)
{
    UART_InitTypeDef uart;
    
    smu_init_m();
    
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.IntEnable = 0;
    uart.clk = get_sysclk();
    uart_init(&uart);
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    while(1);
}