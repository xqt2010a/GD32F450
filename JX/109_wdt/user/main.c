#include "jx_uart.h"
#include "jx_wdt.h"
#include "stdio.h"

#define JX_W4(x)    (*(unsigned int *)(x))

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void SMU_Init(void)
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
	udelay(10000);
}

void main(void)
{
    SMU_Init();
    
    uart_init(115200);
    printf("hello world!\r\n");
    
    
    IWDG_SetReload(15);  //设置的值为1，即65536*2-1 ，在125MHZ的频率下1.05ms后若没有喂狗复位系统
    IWDG_ClearITPendingBit();   //清看门狗中断标志
    IWDG_ReloadCounter();
    IWDG_Enable(0);   //使能看门狗,直接 Reset
    while(1);
}