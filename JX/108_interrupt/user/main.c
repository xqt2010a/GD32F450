#include "jx.h"
#include "jx_uart.h"
#include "jx_int.h"
#include "system.h"
#include "stdio.h"



static void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init1(void)
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
    uint32_t sysclk;
    int i;
    //smu_init1();
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\nsysclk = %dk\r\n",sysclk/1000);
    
    IRQ_Init();
    IRQ_Register(110, uart_irq);
    IRQ_SetEnable();
    
    while(1){
        i = uart_rx_count;
        if(i>0){
            while(uart_rx_count--){
                uart_tx(uart_buf_rx[i-uart_rx_count-1]);
            }
        }
    }
}