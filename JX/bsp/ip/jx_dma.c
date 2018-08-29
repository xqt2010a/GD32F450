#include "jx_dma.h"


void dma_m2m(DMA_Struct *dma_s)
{
//    DMA_CFG(dma_s->dma) = 1;        //enable DMA
//    DMA_CHy_CFG_L(dma_s->dma, dma_s->ch) = (2<<2)|(2);  //shawow register
//    DMA_CHy_CFG_H(dma_s->dma, dma_s->ch) = (1<<3)|(1<<4); //m2m
//    
//    DMA_CHy_SAR(dma_s->dma, dma_s->ch) = dma_s->src;
//    DMA_CHy_DAR(dma_s->dma, dma_s->ch) = dma_s->dst;
//    
//    DMA_CHy_BLK_TS(dma_s->dma, dma_s->ch) = dma_s->len;
//    
//    DMA_CHy_CTL_L(dma_s->dma, dma_s->ch) = (1<<4)|(1<<6)|(dma_s->width << 8)|(dma_s->width << 11)|    //src and dst addr increment
//                                    (dma_s->size<<14)|(dma_s->size << 18);   //set SAR/DAR
//    DMA_CHy_CTL_H(dma_s->dma, dma_s->ch) = (dma_s->len<<7)|(dma_s->len<<16)|(1<<6)|(1<<15);
//    
//    DMA_CHEN(dma_s->dma) = (0x101<<(dma_s->ch-1));
    
    DMA_CFG(0) = 0x1;
    DMA_CHy_CFG_L(0,6) = 0x0;
    DMA_CHy_CFG_H(0,6) = 0x40018;
    DMA_CHy_SAR(0,6) = 0x80000;
    DMA_CHy_DAR(0,6) = 0x90000;
    DMA_CHy_LLP_L(0,6) = 0x97740;
    DMA_CHy_LLP_H(0,6) = 0x0;
    DMA_CHy_QOS(0,6) = 0xb8;
    DMA_CHy_AXI_ID(0,6) = 0x3000c;
    DMA_CHy_BLK_TS(0,6) = 0xb0;
                                         
    DMA_CHy_CTL_L(0,6) = 0x44100;
    DMA_CHy_CTL_H(0,6) = 0x40058040;
    DMA_CHEN(0) = 0x101<<5;
}
