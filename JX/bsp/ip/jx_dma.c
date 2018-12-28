#include "jx_dma.h"


void dma_m2m(DMA_InitTypeDef *dma)
{
    dwc_dma_init(dma);
    dwc_dma_enable(dma);
}
