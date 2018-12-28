#include "jx_uart.h"
#include "jx_wdt.h"
#include "stdio.h"

UART_InitTypeDef uart;
WDT_InitTypeDef  wdt;

void uart_struct_init(void)
{
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.int_mask = 0;
}

void wdt_struct_init(void)
{
    wdt.ch = WDT0;
    wdt.mode = 0;           // 0:system reset 1:interrupt
    wdt.reload_value = CLK_2G;  //���õ�ֵΪ1����65536*2-1 ����125MHZ��Ƶ����1.05ms����û��ι����λϵͳ
    wdt.enable = 1;         // 0:disable 1:enable
}

void main(void)
{
    wdt_struct_init();
    uart_struct_init();
    uart_init(&uart);
    
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    
    wdt_init(&wdt);
    dwc_wdt_enable(&wdt);             //ʹ�ܿ��Ź�,ֱ�� Reset
    while(1);
}