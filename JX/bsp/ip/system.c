//#include "jx.h"

//extern int __ICFEDIT_region_ROM_start__;

void SystemInit (void)
{
    unsigned char i;
    //unsigned int rom_addr = (unsigned int)&__ICFEDIT_region_ROM_start__;
    
    for(i=0; i<8; i++){
        //(*(unsigned int *)(i * 4)) = (*(unsigned int *)(rom_addr + i * 4));
        (*(unsigned int *)(i * 4)) = (*(unsigned int *)(0x80000 + i * 4));
    }
}


