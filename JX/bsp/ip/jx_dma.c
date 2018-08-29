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
    
    DMA_CFG(0) = 0x3;
    //(*(volatile unsigned int*)(0x190e014)) = 0x0;
    (*(volatile unsigned int*)(0x190e620)) = 0x0;
    (*(volatile unsigned int*)(0x190e624)) = 0x40018;
    (*(volatile unsigned int*)(0x190e600)) = 0x80000;
    (*(volatile unsigned int*)(0x190e604)) = 0x0;
    (*(volatile unsigned int*)(0x190e608)) = 0x90000;
    (*(volatile unsigned int*)(0x190e60c)) = 0x0;
    (*(volatile unsigned int*)(0x190e628)) = 0x97740;
    (*(volatile unsigned int*)(0x190e62c)) = 0x0;
    (*(volatile unsigned int*)(0x190e658)) = 0xb8;
    (*(volatile unsigned int*)(0x190e65c)) = 0x0;
    (*(volatile unsigned int*)(0x190e650)) = 0x3000c;
    (*(volatile unsigned int*)(0x190e654)) = 0x0;
    (*(volatile unsigned int*)(0x190e610)) = 0xb0;
    (*(volatile unsigned int*)(0x190e614)) = 0x0;
                                         
    (*(volatile unsigned int*)(0x190e618)) = 0x44100;
    (*(volatile unsigned int*)(0x190e61c)) = 0x40058040;
    (*(volatile unsigned int*)(0x190e018)) = 0x2020;
    (*(volatile unsigned int*)(0x190e01c)) = 0x0;
}
