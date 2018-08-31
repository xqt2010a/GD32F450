#include "ddr_test.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "jx_dma.h"

#define DDR_SEED        0x12345678
#define DDR_SIZE_1      0x40000000
#define DDR_SIZE_4      0x10000000
#define DDR_BLK_SIZE    0x1000
#define DDR_DMA_TX_SIZE (DDR_BLK_SIZE*32)   //dma transfer bytes every time
#define DDR_BLK_N       (DDR_SIZE_1/2/DDR_DMA_TX_SIZE)  //block num in 512M ddr 

#define DDR_BASE1       0x80000000
#define DDR_BASE2       0xa0000000     //0xc0000000
#define DDR_BASE        0x80000000

#define WIDTH_N(x)      (1<<(3+x))
#define DDR_RW1(x)      (*(volatile unsigned char*)(x))
#define DDR_RW4(x)      (*(volatile unsigned int*)(x))

uint32_t Dst_Base[] = {0x80000000, 0x88000000, 0x90000000, 0x98000000};
uint32_t Src_Base[] = {0xa0000000, 0xa8000000, 0xb0000000, 0xb8000000};

void ddr_write_order_w1(uint32_t addr, uint32_t len)
{
    uint32_t i;
    for(i=0; i<len; i++){
        DDR_RW1(addr+i) = i;
    }
    printf("info: order w1 0x%x 0x%xB=%dKB=%dMB finish.\r\n", addr, len, len/1024, len/0x100000);
}

void ddr_write_order_w4(uint32_t addr, uint32_t len)
{
    uint32_t i;
    for(i=0; i<len; i++){
        DDR_RW4(addr+i*4) = i;
    }
    printf("info: order w4 0x%x 0x%xB=%dKB=%dMB finish.\r\n", addr, len*4, len/256, len/0x40000);
}

void ddr_write_rand_w1(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i;
    srand(seed);
    for(i=0; i<len; i++){
        DDR_RW1(addr+i) = rand();
    }
    printf("info: rand w1 0x%x 0x%xB=%dKB=%dMB finish.\r\n", addr, len, len/1024, len/0x100000);
}

void ddr_write_rand_w4(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i;
    srand(seed);
    for(i=0; i<len; i++){
        DDR_RW4(addr+i*4) = rand();
    }
    printf("info: rand w4 0x%x 0x%xB=%dKB=%dMB finish.\r\n", addr, len*4, len/256, len/0x40000);
}

void ddr_read_order_rl(uint32_t addr, uint32_t len)
{
    uint32_t i, data;
    for(i=0; i<len; i++){
        data = DDR_RW1(addr+i);
        if(data != (i & 0xFF)){
            printf("error: order r1 dst:0x%x=0x%x != 0x%x.\r\n", addr+i, data, (i&0xFF));
            while(1);
        }
    }
    printf("success: order r1 dst:0x%x 0x%xB==%dKB=%dMB.\r\n", addr, len, len/1024, len/0x100000);
}

void ddr_read_order_r4(uint32_t addr, uint32_t len)
{
    uint32_t i, data;
    for(i=0; i<len; i++){
        data = DDR_RW4(addr+i*4);
        if(data != i){
            printf("error: order r4 dst:0x%x=0x%x != 0x%x.\r\n", addr+i, data, i);
            while(1);
        }
    }
    printf("success: order r4 dst:0x%x 0x%xB==%dKB=%dMB.\r\n", addr, len*4, len/256, len/0x40000);
}

void ddr_read_rand_rl(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i, data[2];
    srand(seed);
    for(i=0; i<len; i++){
        data[0] = DDR_RW1(addr+i);
        data[1] = rand()&0xFF;
        if(data[0] != data[1]){
            printf("error: rand r1 seed:0x%x dst:0x%x=0x%x != 0x%x.\r\n", seed, addr+i, data[0], data[1]);
            while(1);
        }
    }
    printf("success: rand r1 seed:0x%x dst:0x%x 0x%xB==%dKB=%dMB.\r\n", seed, addr, len, len/1024, len/0x100000);
}

void ddr_read_rand_r4(uint32_t seed, uint32_t addr, uint32_t len)
{
    uint32_t i, data[2];
    srand(seed);
    for(i=0; i<len; i++){
        data[0] = DDR_RW4(addr+i*4);
        data[1] = rand();
        if(data[0] != data[1]){
            printf("error: rand r4 seed:0x%x dst:0x%x=0x%x != 0x%x.\r\n", seed, addr+i*4, data[0], data[1]);
            while(1);
        }
    }
    printf("success: rand r4 seed:0x%x dst:0x%x 0x%xB==%dKB=%dMB.\r\n", seed, addr, len*4, len/256, len/0x40000);
}

void ddr_ds_r1(uint32_t dst_addr, uint32_t src_addr, uint32_t len)
{
    uint32_t i;
    unsigned char data[2];
    for(i=0; i<len; i++){
        data[0] = DDR_RW1(dst_addr+i);
        data[1] = DDR_RW1(src_addr+i);
        if(data[0] != data[1]){
            printf("error: dst:0x%x=0x%x src:0x%x=0x%x.\r\n", dst_addr+i, data[0], src_addr+i, data[1]);
            while(1);
        }
    }
    printf("success: r1 dst:0x%x, src:0x%x 0x%xB=%dKB=%dMB.\r\n", dst_addr, src_addr, len, len/1024, len/0x100000);
}

void ddr_ds_r4(uint32_t dst_addr, uint32_t src_addr, uint32_t len)
{
    uint32_t i;
    uint32_t data[2];
    for(i=0; i<len; i++){
        data[0] = DDR_RW4(dst_addr+i*4);
        data[1] = DDR_RW4(src_addr+i*4);
        if(data[0] != data[1]){
            printf("error: dst:0x%x=0x%x src:0x%x=0x%x.\r\n", dst_addr+i*4, data[0], src_addr+i*4, data[1]);
            while(1);
        }
    }
    printf("success: r4 dst:0x%x, src:0x%x 0x%xB=%dKB=%dMB.\r\n", dst_addr, src_addr, len*4, len/256, len/0x40000);
}


void ddr_cpu_order(uint32_t addr, uint32_t len)
{
    //ddr_write_order_w1(addr, len);
    //ddr_read_order_r1(addr, len);
    
    ddr_write_order_w4(addr, len);
    ddr_read_order_r4(addr, len);
}

void ddr_cpu_rand(uint32_t seed, uint32_t addr, uint32_t len)
{
    //ddr_write_rand_w1(seed, addr, len);
    //ddr_read_rand_r1(seed, addr, len);
    
    ddr_write_rand_w4(seed, addr, len);
    ddr_read_rand_r4(seed, addr, len);
}

void ddr_cpu_test(void)
{
    ddr_cpu_order(DDR_BASE, DDR_SIZE_4);
    ddr_cpu_rand(DDR_SEED, DDR_BASE, DDR_SIZE_4);
}

void ddr_dma_fixed_test(void)
{
    unsigned int i;
    DMA_Struct dma_s;
    dma_s.dma = 0;
    dma_s.ch = 1;
    dma_s.width = 5;
    dma_s.size = SIZE_1024;
    dma_s.dst = 0x80000000;
    dma_s.src = 0xa0000000;
    dma_s.len = 0x3FFFFF;
    ddr_write_order_w4(dma_s.src, 0x4000000); //256M
    dma_m2m(&dma_s);
    while(1){
        printf("0x%x\r\n",DMA_CHy_STATUS(dma_s.dma, dma_s.ch)&0x3FFFFF);
        
        for(i=0; i<0xFFFFFF; i++);
    }
}

void ddr_dma_write_128KB(uint8_t dma, uint8_t ch, uint32_t dst, uint32_t src)
{
    DMA_Struct dma_s;
    dma_s.dma = dma;
    dma_s.ch = ch;
    dma_s.width = WIDTH_256;
    dma_s.size = SIZE_1024;
    dma_s.dst = dst;
    dma_s.src = src;
    dma_s.len = 0xFFF;
    dma_m2m(&dma_s);
}

void ddr_dma_write_2MB(uint32_t dst, uint32_t src)
{
    uint8_t i;
    for(i=0; i<8; i++){
        ddr_dma_write_128KB(0, i+1, dst+0x20000*i, src+0x20000*i);
        ddr_dma_write_128KB(1, i+1, dst+0x100000+0x20000*i, src+0x100000+0x20000*i);
    }
    for(i=0; i<8; i++){
        while((DMA_CHy_STATUS(0,i+1) & 0xFFF) != 0xFFF);
        while((DMA_CHy_STATUS(1,i+1) & 0xFFF) != 0xFFF);
    }
    printf("info: dma transfer dst:0x%x src:0x%x 2MB finish.\r\n", dst, src);
}

void ddr_dma_order(uint32_t dst, uint32_t src, uint32_t bytes)
{
    uint32_t i;
    ddr_write_order_w4(src, bytes/4);
    for(i=0; i<(bytes/0x200000); i++){
        ddr_dma_write_2MB(dst+0x200000*i, src+0x200000*i);
    }
    ddr_ds_r4(dst, src, bytes/4);
}

void ddr_dma_rand(uint32_t seed, uint32_t dst, uint32_t src, uint32_t bytes)
{
    uint32_t i;
    ddr_write_rand_w4(seed, src, bytes/4);
    for(i=0; i<(bytes/0x200000); i++){
        ddr_dma_write_2MB(dst+0x200000*i, src+0x200000*i);
    }
    ddr_ds_r4(dst, src, bytes/4);
}

void ddr_dma_test(void)
{
    ddr_dma_order(DDR_BASE1, DDR_BASE2, DDR_SIZE_1/2);
    ddr_dma_order(DDR_BASE2, DDR_BASE1, DDR_SIZE_1/2);
    ddr_dma_rand(DDR_SEED, DDR_BASE1, DDR_BASE2, DDR_SIZE_1/2);
    ddr_dma_rand(DDR_SEED, DDR_BASE2, DDR_BASE1, DDR_SIZE_1/2);
}

void ddr_dma_OutOfOrder(uint32_t seed, uint32_t dst, uint32_t src, uint32_t bytes)
{
    DMA_Struct dma_s;
    
    srand(seed);
    dma_s.dma = rand() % 2;
    dma_s.ch = (rand() % 8) + 1;
    dma_s.width = rand() % 6;
    dma_s.size = SIZE_1024;
    dma_s.dst = dst;
    dma_s.src = src;
    dma_s.len = bytes;
    bytes = dma_s.len*(WIDTH_N(dma_s.width)/8);
    ddr_write_rand_w1(seed, src, bytes);
    dma_m2m(&dma_s);
    while((DMA_CHy_STATUS(dma_s.dma, dma_s.ch)&0x3FFFFF) != dma_s.len);
    printf("seed=0x%x, dma%d ch%d width=%d, dst=0x%x, src=0x%x, len=%d, bytes=0x%x=%dk\r\n",seed, dma_s.dma,
           dma_s.ch, WIDTH_N(dma_s.width), dma_s.dst, dma_s.src, dma_s.len, bytes, bytes/1024);
    ddr_ds_r1(dst, src, bytes);
}

void ddr_dma_OutOfOrder_test(void)
{
    uint32_t i, data, dst, src, bytes;
    srand(DDR_SEED);
    for(i=0; i<0x2000; i++){    //512M/(128k/2)
        data = rand();
        dst = DDR_BASE1 + DDR_DMA_TX_SIZE * (data % DDR_BLK_N);
        src = DDR_BASE2 + DDR_DMA_TX_SIZE * (data % DDR_BLK_N);
        bytes = data & 0xFFF;
        ddr_dma_OutOfOrder(data, dst, src, bytes);
    }
    for(i=0; i<0x2000; i++){    //512M/(128k/2)
        data = rand();
        dst = DDR_BASE2 + DDR_DMA_TX_SIZE * (data % DDR_BLK_N);
        src = DDR_BASE1 + DDR_DMA_TX_SIZE * (data % DDR_BLK_N);
        bytes = data & 0xFFF;
        ddr_dma_OutOfOrder(data, dst, src, bytes);
    }
}