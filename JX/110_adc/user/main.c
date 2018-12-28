#include "system.h"
#include "jx_uart.h"
#include "jx_adc.h"
#include "stdio.h"

UART_InitTypeDef uart;
ADC_InitTypeDef adc;

void adc_struct_init(void)
{
    adc.ch = ADC0;
    adc.pin = 0;
    adc.mode = MODE_11BIT;
    adc.int_mask = 0;   //0:disable int 1:enable int
    adc.sample_cnt = 0xa;
}

void uart_struct_init(void)
{
    uart.ch = UART1;
    uart.bandrate = 115200;
    uart.int_mask = 0;
}

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void main(void)
{
    uint32_t voltage[24];//,i;
    
    adc_struct_init();
    uart_struct_init();
    uart_init(&uart);
    dwc_adc_init(&adc);
    
    printf("hello world!\r\nsysclk = %dk\r\n",uart.clk/1000);
    while(1){
        voltage[0] = dwc_adc_get(&adc);
        printf("value: %3x\r\n", voltage[0]);
        udelay(100000);
    }
}