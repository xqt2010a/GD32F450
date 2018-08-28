#include "jx_dma.h"


void dma_m2m(DMA_Struct dma_s)
{
    DMA_CFG(dma_s.dma) = 1;        //enable DMA
    DMA_CHy_CTL(dma_s.dma, dma_s.ch) = (1<<4)|(1<<6)|(dma_s.width << 8)|(dma_s.width << 11)|    //src and dst addr increment
                                    (dma_s.size<<14)|(dma_s.size << 18);   //set SAR/DAR
    DMA_CHy_SAR(dma_s.dma, dma_s.ch) = dma_s.src;
    DMA_CHy_DAR(dma_s.dma, dma_s.ch) = dma_s.dst;
    DMA_CHy_CFG(dma_s.dma, dma_s.ch) = (1<<35)|(1<<36); //m2m
    DMA_CHy_BLK_TS(dma_s.dma, dma_s.ch) = dma_s.len;
    DMA_CHEN(dma_s.dma) = (1<<dma_s.ch);
}