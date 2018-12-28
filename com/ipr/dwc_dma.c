#include "dwc_dma.h"

void dwc_dma_init(DMA_InitTypeDef *dma)
{
    DMA_CFG(dma->ch) = 1;                    //enable DMA
    DMA_CHy_CFG_L(dma->ch, dma->n) = 0;   //shawow register
    DMA_CHy_CFG_H(dma->ch, dma->n) = (dma->type)|(1<<3)|(1<<4); //m2m
    
    DMA_CHy_SAR(dma->ch, dma->n) = dma->src;
    DMA_CHy_DAR(dma->ch, dma->n) = dma->dst;
    
    DMA_CHy_BLK_TS(dma->ch, dma->n) = dma->len;     //max = 1024*1024*4 = 4M
    
    DMA_CHy_CTL_L(dma->ch, dma->n) = (dma->width << 8)|(dma->width << 11)|    //src and dst addr increment
                                    (dma->size<<14)|(dma->size << 18);   //set SAR/DAR
    //DMA_CHy_CTL_H(dma_s->dma, dma_s->ch) = (dma_s->len<<7)|(dma_s->len<<16)|(1<<6)|(1<<15);
    
    
}

void dwc_dma_enable(DMA_InitTypeDef *dma)
{
    DMA_CHEN(dma->ch) = (0x101<<(dma->n-1));
}