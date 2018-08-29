#ifndef __JX_DMA_H__
#define __JX_DMA_H__

#include "stdint.h"

#define DMA_STRUCT_FLAG     0

#define DMA_BASE        0x0190E000
#define DMA0_BASE       (DMA_BASE)
#define DMA1_BASE       (DMA_BASE + 0x1000)

#define DMA_ID(x)                   (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x000))
#define DMA_COMPVER(x)              (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x008))
#define DMA_CFG(x)                  (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x010))
#define DMA_CHEN(x)                 (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x018))
#define DMA_INTSTATUS(x)            (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x030))
#define DMA_COM_INTCLR(x)           (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x038))
#define DMA_COM_INTSTATUS_EN(x)     (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x040))
#define DMA_COM_INTSIGNAL_EN(x)     (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x048))
#define DMA_COM_INTSTATUS(x)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x050))
#define DMA_RST(x)                  (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x058))
#define DMA_CHy_SAR(x,y)            (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x00)) // y = 1 to 8
#define DMA_CHy_DAR(x,y)            (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x08))
#define DMA_CHy_BLK_TS(x,y)         (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x10))
#define DMA_CHy_CTL_L(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x18))
#define DMA_CHy_CTL_H(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x1C))
#define DMA_CHy_CFG_L(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x20))
#define DMA_CHy_CFG_H(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x24))
#define DMA_CHy_LLP_L(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x28))
#define DMA_CHy_LLP_H(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x2C))
#define DMA_CHy_STATUS(x,y)         (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x30))
#define DMA_CHy_SWHSSRC(x,y)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x38))
#define DMA_CHy_SWHSDST(x,y)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x40))
#define DMA_CHy_BLK_TFR(x,y)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x48))
#define DMA_CHy_AXI_ID(x,y)         (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x50))
#define DMA_CHy_QOS(x,y)            (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x58))
#define DMA_CHy_SSTAT(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x60))
#define DMA_CHy_DSTAT(x,y)          (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x68))
#define DMA_CHy_SSTATAR(x,y)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x70))
#define DMA_CHy_DSTATAR(x,y)        (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x78))
#define DMA_CHy_INTSTATUS_EN(x,y)   (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x80))
#define DMA_CHy_INTSTATUS(x,y)      (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x88))
#define DMA_CHy_INTSIGNAL_EN(x,y)   (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x90))
#define DMA_CHy_INTCLR(x,y)         (*(volatile unsigned int*)(DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x98))

typedef enum {
    DMA0 = 0,
    DMA1 = 1
} DMAn;

typedef enum {
    CH1 = 1,
    CH2 = 2,
    CH3 = 3,
    CH4 = 4,
    CH5 = 5,
    CH6 = 6,
    CH7 = 7,
    CH8 = 8
} CHn;

typedef enum {
    WIDTH_8   = 0x0,
    WIDTH_16  = 0x1,
    WIDTH_32  = 0x2,
    WIDTH_64  = 0x3,
    WIDTH_128 = 0x4,
    WIDTH_256 = 0x5,    //max
    WIDTH_512 = 0x6
} WIDTHn;

typedef enum {
    SIZE_1       = 0x0,
    SIZE_4       = 0x1,
    SIZE_8       = 0x2,
    SIZE_16      = 0x3,
    SIZE_32      = 0x4,
    SIZE_64      = 0x5,
    SIZE_128     = 0x6,
    SIZE_256     = 0x7,
    SIZE_512     = 0x8,
    SIZE_1024    = 0x9
} SIZEn;

#if DMA_STRUCT_FLAG
typedef struct{
    DMAn        dma;
    CHn         ch;
    WIDTHn      width;
    SIZEn       size;
    uint32_t    dst;    //destination address
    uint32_t    src;    //source address
    uint32_t    len;    //
} DMA_Struct;

#else

typedef struct{
    unsigned char   dma;
    unsigned char   ch;
    unsigned char   width;
    unsigned char   size;
    uint32_t        dst;    //destination address
    uint32_t        src;    //source address
    uint32_t        len;    //
} DMA_Struct;

#endif

void dma_m2m(DMA_Struct *dma_s);

#endif  /* __JX_DMA_H__ */