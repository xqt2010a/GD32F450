#include "ddr_dma_test.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

#define DDR_DMA_SEED    0x12345678
#define DDR_WR_BYTE(x)   (*(volatile unsigned char*)(x))
#define WIDTH_N(x)      (1<<(3+x))

uint32_t Dst_Base[] = {0x80000000, 0x88000000, 0x90000000, 0x98000000};
uint32_t Src_Base[] = {0xa0000000, 0xa8000000, 0xb0000000, 0xb8000000};

void ddr_write(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i;
    unsigned char data;
    srand(seed);
    for(i=0; i<len; i++){
        data = rand() & 0xFF;
        DDR_WR_BYTE(addr+i) = data;
    }
}

void ddr_read(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i;
    unsigned char data;
    srand(seed);
    for(i=0; i<len; i++){
        data = rand() & 0xFF;
        if(DDR_WR_BYTE(addr+i) != data){
            printf("error: checked error at 0x%x\r\n",(addr+i));
        }
    }
    printf("success: seed=0x%x, addr=ox%x, len=%d\r\n",seed, addr, len);
}

void ddr_dma_test(uint32_t seed)
{
    uint32_t data, bytes;
    DMA_Struct dma_s;
    
    srand(seed);
    data = rand();
    dma_s.dma = data % 2;
    dma_s.ch = (data % 8) + 1;
    dma_s.width = data % 6;
    dma_s.size = SIZE_1024;
    dma_s.dst = Dst_Base[data%4];
    dma_s.src = Src_Base[data%4];
    dma_s.len = data & 0x3FFFFF;
    
    bytes = dma_s.len*(WIDTH_N(dma_s.width)/8);
    printf("seed=0x%x, dma%d ch%d width=%d, dst=0x%x, src=0x%x, len=%d, bytes=%d=%dk\r\n",seed, dma_s.dma,
           dma_s.ch, WIDTH_N(dma_s.width), dma_s.dst, dma_s.src, dma_s.len, bytes, bytes/1024);
    ddr_write(seed, dma_s.src, bytes);
    printf("write data finish.\r\n");
    dma_m2m(&dma_s);
    printf("%d",DMA_CHy_STATUS(dma_s.dma, dma_s.ch)&0x3FFFFF);
    ddr_read(seed, dma_s.dst, bytes);
}
    