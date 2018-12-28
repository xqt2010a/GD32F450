#ifndef __JX_DMA_H__
#define __JX_DMA_H__

#include "dwc_dma.h"

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

typedef enum {
    M2M_DMAC    = 0x0,
    M2P_DMAC    = 0x1,
    P2M_DMAC    = 0x2,
    P2P_DMAC    = 0x3,
    P2M_P       = 0x4,
    P2P_SP      = 0x5,
    M2P_P       = 0x6,
    P2P_DP      = 0x7
} TYPES_FC;

void dma_m2m(DMA_InitTypeDef *dma);

#endif  /* __JX_DMA_H__ */