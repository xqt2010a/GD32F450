#include "jx_uart.h"
#include "jx_dma.h"
#include "stdio.h"

#define JX_W4(x)    (*(unsigned int *)(x))

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

unsigned char Dst_buf[32]="hello world ! code by labc.";
unsigned char Src_buf[32];

void main(void)
{
    smu_init();
    DMA_Struct dma_s;
    dma_s.dma = DMA0;
    dma_s.ch = CH1;
    dma_s.width = WIDTH_8;
    dma_s.size = SIZE_8;
    dma_s.dst = (uint32_t)Dst_buf;
    dma_s.src = (uint32_t)Src_buf;
    dma_s.len = 2;
    
    dma_m2m(&dma_s);
    while(1);
}