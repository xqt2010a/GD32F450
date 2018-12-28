#include "jx_uart.h"
#include "jx_dma.h"
#include "stdio.h"

unsigned char Src_buf[64]="hello world ! code by labc.hhhhhhsghgsgdsjagjgfjagfa";
unsigned char Dst_buf[64];

void main(void)
{
    DMA_Struct dma_s;
    dma_s.dma = DMA0;
    dma_s.ch = CH1;
    dma_s.width = WIDTH_8;
    dma_s.size = SIZE_1024;
    dma_s.dst = (uint32_t)Dst_buf;
    dma_s.src = (uint32_t)Src_buf;
    dma_s.len = 2;
    
    dma_m2m(&dma_s);
    while(1);
}