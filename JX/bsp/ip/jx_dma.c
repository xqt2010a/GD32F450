#include "jx_dma.h"


void dma_m2m(DMA_Struct *dma_s)
{
    DMA_CFG(dma_s->dma) = 1;                    //enable DMA
    DMA_CHy_CFG_L(dma_s->dma, dma_s->ch) = 0;   //shawow register
    DMA_CHy_CFG_H(dma_s->dma, dma_s->ch) = (1<<3)|(1<<4); //m2m
    
    DMA_CHy_SAR(dma_s->dma, dma_s->ch) = dma_s->src;
    DMA_CHy_DAR(dma_s->dma, dma_s->ch) = dma_s->dst;
    
    DMA_CHy_BLK_TS(dma_s->dma, dma_s->ch) = dma_s->len;     //max = 1024*1024*4 = 4M
    
    DMA_CHy_CTL_L(dma_s->dma, dma_s->ch) = (dma_s->width << 8)|(dma_s->width << 11)|    //src and dst addr increment
                                    (dma_s->size<<14)|(dma_s->size << 18);   //set SAR/DAR
    //DMA_CHy_CTL_H(dma_s->dma, dma_s->ch) = (dma_s->len<<7)|(dma_s->len<<16)|(1<<6)|(1<<15);
    
    DMA_CHEN(dma_s->dma) = (0x101<<(dma_s->ch-1));
}
