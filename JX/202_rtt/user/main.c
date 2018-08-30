#include "SEGGER_RTT.h"
#include "stdio.h"
#include "stdint.h"

#define JX_W4(x)    (*(unsigned int *)(x))

#define BUF_LEN  64
#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)
uint8_t  Rx_size;
uint8_t  Rx_buf[BUF_LEN];


void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init(void)
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
    smu_init();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    printf("RTT demo!\r\n");
    while(1);
}