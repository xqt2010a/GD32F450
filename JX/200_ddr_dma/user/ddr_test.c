#include "ddr_test.h"
#include "stdlib.h"

#define DDR_SEED     0x12345678

#define DDR_BASE1    0x80000000
#define DDR_BASE2    0xa0000000     //0xc0000000
#define DDR_WR_1(x)   (*(volatile unsigned long*)(DDR_BASE1 + 4*x))
#define DDR_WR_2(x)   (*(volatile unsigned long*)(DDR_BASE2 + 4*x))

#define DDR_BASE    0x80000000
#define DDR_WR(x)   (*(volatile unsigned long*)(DDR_BASE + 4*x))

unsigned int count=0;

void ddr_order_discontinuous(void)  //512+512
{
    unsigned int i,data1, data2;
    for(i=0; i<0x08000000; i++){
        DDR_WR_1(i) = i;
        DDR_WR_2(i) = i;
        data1 = DDR_WR_1(i);
        data2 = DDR_WR_2(i);
        if(DDR_WR_1(0) != 0){
            count = i;
            while(1);
        }
        if(DDR_WR_2(0) != 0){
            count = i;
            while(1);
        }
    }
    
    for(i=0; i<0x08000000; i++){
        data1 = DDR_WR_1(i);
        data2 = DDR_WR_2(i);
        if(DDR_WR_1(i) != i){
            count = i;
            while(1);
        }
        if(DDR_WR_2(i) != i){
            count = i;
            while(1);
        }
    }
    data1 = data1;
    data2 = data2;
}

void ddr_rand_discontinuous(void)   //512+512
{
    unsigned int i, data;
    srand(DDR_SEED);
    for(i=0; i<0x08000000; i++){
        data = rand();
        DDR_WR_1(i) = data;
        DDR_WR_2(i) = data;
    }
    srand(DDR_SEED);
    for(i=0; i<0x08000000; i++){
        data = rand();
        if(data != DDR_WR_1(i)){
            count = i;
            while(1);
        }
        if(data != DDR_WR_2(i)){
            count = i;
            while(1);
        }
    }
}

void ddr_order(void)    //1GB
{
    unsigned int i, data;
    for(i=0; i<0x10000000; i++){
        DDR_WR(i) = i;
        if(DDR_WR(0) != 0){
            count = i;
            while(1);
        }
    }
    for(i=0; i<0x10000000; i++){
        data = DDR_WR(i);
        if(data != i){
            count = i;
            while(1);
        }
    }
}

void ddr_rand(void) //1GB
{
    unsigned int i, data;
    srand(DDR_SEED);
    for(i=0; i<0x10000000; i++){
        data = rand();
        DDR_WR(i) = data;
    }
    srand(DDR_SEED);
    for(i=0; i<0x10000000; i++){
        data = rand();
        if(data != DDR_WR(i)){
            count = i;
            while(1);
        }
    }
}