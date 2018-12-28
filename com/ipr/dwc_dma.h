#ifndef __DWC_DMA_H__
#define __DWC_DMA_H__

#include "config.h"

#define DMA_BASE(x)     (SYS_DMA_BASE+0x1000*(x))

#define DMA_ID(ch)                   (*(volatile unsigned int *)(DMA_BASE(ch) + 0x000))
#define DMA_COMPVER(ch)              (*(volatile unsigned int *)(DMA_BASE(ch) + 0x008))
#define DMA_CFG(ch)                  (*(volatile unsigned int *)(DMA_BASE(ch) + 0x010))
#define DMA_CHEN(ch)                 (*(volatile unsigned int *)(DMA_BASE(ch) + 0x018))
#define DMA_INTSTATUS(ch)            (*(volatile unsigned int *)(DMA_BASE(ch) + 0x030))
#define DMA_COM_INTCLR(ch)           (*(volatile unsigned int *)(DMA_BASE(ch) + 0x038))
#define DMA_COM_INTSTATUS_EN(ch)     (*(volatile unsigned int *)(DMA_BASE(ch) + 0x040))
#define DMA_COM_INTSIGNAL_EN(ch)     (*(volatile unsigned int *)(DMA_BASE(ch) + 0x048))
#define DMA_COM_INTSTATUS(ch)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x050))
#define DMA_RST(ch)                  (*(volatile unsigned int *)(DMA_BASE(ch) + 0x058))
#define DMA_CHy_SAR(ch,n)            (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x00)) // y = 1 to 8
#define DMA_CHy_DAR(ch,n)            (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x08))
#define DMA_CHy_BLK_TS(ch,n)         (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x10))
#define DMA_CHy_CTL_L(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x18))
#define DMA_CHy_CTL_H(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x1C))
#define DMA_CHy_CFG_L(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x20))
#define DMA_CHy_CFG_H(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x24))
#define DMA_CHy_LLP_L(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x28))
#define DMA_CHy_LLP_H(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x2C))
#define DMA_CHy_STATUS(ch,n)         (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x30))
#define DMA_CHy_SWHSSRC(ch,n)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x38))
#define DMA_CHy_SWHSDST(ch,n)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x40))
#define DMA_CHy_BLK_TFR(ch,n)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x48))
#define DMA_CHy_AXI_ID(ch,n)         (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x50))
#define DMA_CHy_QOS(ch,n)            (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x58))
#define DMA_CHy_SSTAT(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x60))
#define DMA_CHy_DSTAT(ch,n)          (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x68))
#define DMA_CHy_SSTATAR(ch,n)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x70))
#define DMA_CHy_DSTATAR(ch,n)        (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x78))
#define DMA_CHy_INTSTATUS_EN(ch,n)   (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x80))
#define DMA_CHy_INTSTATUS(ch,n)      (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x88))
#define DMA_CHy_INTSIGNAL_EN(ch,n)   (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x90))
#define DMA_CHy_INTCLR(ch,n)         (*(volatile unsigned int *)(DMA_BASE(ch) + 0x100*(n) + 0x98))

/**
  * @brief  DMA Init structure definition
  */
  
typedef struct
{
    uint8_t     ch;
    uint8_t     n;
    uint8_t     type;   //0:mm 1:mp 2:pm 3:pp 4:pm 5:pp 6:mp 7:pp
    uint8_t     width;
    uint8_t     size;
    uint32_t    dst;    //destination address
    uint32_t    src;    //source address
    uint32_t    len;    //
} DMA_InitTypeDef;

void dwc_dma_init(DMA_InitTypeDef *dma);

#endif  /* __DWC_DMA_H__ */