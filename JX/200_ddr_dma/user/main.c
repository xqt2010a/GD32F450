#include "stdio.h"
#include "ddr.h"
#include "ddr_test.h"
#include "jx_uart.h"
#include "system.h"
#include "lpddr4_init_training_pass.h"


#define JX_W4(x)    (*(unsigned int *)(x))

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init1(void)
{
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08104) = 0xFFFFFFFF;
    JX_W4(0x0190d100) = 0xFFFFFFFF;
    JX_W4(0x0190d104) = 0xFFFFFFFF;
    JX_W4(0x0190d108) = 0xFFFFFFFF;
    JX_W4(0x0190d10c) = 0xFFFFFFFF;
    JX_W4(0x0190d110) = 0xFFFFFFFF;
    JX_W4(0x0190d114) = 0xFFFFFFFF;
    JX_W4(0x0190d120) = 0xFFFFFFFF;
    JX_W4(0x0190d124) = 0xFFFFFFFF;
    
	/* Delay for somewhile to wait reset de-assertion to be stable. */
	udelay(1000);
}

void main(void)
{
    uint32_t sysclk;
    smu_init1();
    set_ddrc_freq(800); //before other device setting
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\n");
    ddr_init();
    
    //ddr_cpu_test();
    ddr_dma_test();
    ddr_dma_OutOfOrder_test();
    
    while(1);
}