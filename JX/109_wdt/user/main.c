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
    wdt.reload_value = CLK_2G;  //设置的值为1，即65536*2-1 ，在125MHZ的频率下1.05ms后若没有喂狗复位系统
    wdt.enable = 1;         // 0:disable 1:enable
}

void main(void)
{
    wdt_struct_init();
    uart_struct_init();
    uart_init(&uart);
    
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    
    wdt_init(&wdt);
    dwc_wdt_enable(&wdt);             //使能看门狗,直接 Reset
    while(1);
}