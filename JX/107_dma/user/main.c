#include "jx_uart.h"
#include "jx_dma.h"
#include "stdio.h"

unsigned char Src_buf[64]="hello world ! code by labc.hhhhhhsghgsgdsjagjgfjagfa";
unsigned char Dst_buf[64];

void main(void)
{
    DMA_InitTypeDef dma;
    dma.ch = DMA0;
    dma.n = CH1;
    dma.type = M2M_DMAC;
    dma.width = WIDTH_8;
    dma.size = SIZE_1024;
    dma.dst = (uint32_t)Dst_buf;
    dma.src = (uint32_t)Src_buf;
    dma.len = 3;
    
    dma_m2m(&dma);
    while(1);
}