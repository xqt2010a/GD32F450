#include "ddr_test.h"


#define DDR_BASE    0xc0000000
#define DDR_WR(x)   (*(volatile unsigned long*)(DDR_BASE + 4*x))

unsigned int count=0;

void ddr_write(void)
{
    unsigned int i,data;
    for(i=0; i<0x08000000; i++){
        DDR_WR(i) = i;
        data = DDR_WR(i); 
        if(DDR_WR(0) != 0){
            count = i;
            while(1);
        }
        i=i;
    }
    
    for(i=0; i<0x10000000; i++){
        data = DDR_WR(i);
        if(DDR_WR(i) != i){
            while(1);
        }
    }
    data = data;
}

void ddr_read(void)
{

}