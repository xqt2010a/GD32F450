#include "jx.h"
#include "stdint.h"
#include "system.h"

static void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init(void)
{
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08100) = 0xFFFFFFFF;
    JX_WR4(0x3FE08104) = 0xFFFFFFFF;
    JX_WR4(0x0190d100) = 0xFFFFFFFF;
    JX_WR4(0x0190d104) = 0xFFFFFFFF;
    JX_WR4(0x0190d108) = 0xFFFFFFFF;
    JX_WR4(0x0190d10c) = 0xFFFFFFFF;
    JX_WR4(0x0190d110) = 0xFFFFFFFF;
    JX_WR4(0x0190d114) = 0xFFFFFFFF;
    JX_WR4(0x0190d120) = 0xFFFFFFFF;
    JX_WR4(0x0190d124) = 0xFFFFFFFF;
    
	/* Delay for somewhile to wait reset de-assertion to be stable. */
	udelay(10000);
}

void smu_ddr_freq(unsigned int freq_MHZ)
{
    JX_WR4(0x0190d02c) = 0;                  //: make all clock mux select 24M
    JX_WR4(0x0190d068) = 0x80008000;    //: Release DDR Pin strap
    JX_WR4(0x0190d000) = 0xf0;          //: DDR PLL PowerDown and Bypass Mode(24M)
    switch(freq_MHZ)
    {
        /* Set the PHY input clocks back to pstate 0 -- 800MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
    case  3200 :
        JX_WR4(0x0190d000) = 0x0C1909f0;
        JX_WR4(0x0190d000) = 0x0C190900;
        break;
	/* Set the PHY input clocks back to pstate 0 -- 200MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
    case  800 :					
        JX_WR4(0x0190d000) = 0x0C0649f0;
        JX_WR4(0x0190d000) = 0x0C064900;
        break;
    case  1600 :
        /* Set the PHY input clocks back to pstate 0 -- 400MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
        JX_WR4(0x0190d000) = 0x0C0C89f0;
        JX_WR4(0x0190d000) = 0x0C0C8900;
        break;
    case  2400 :
        /* Set the PHY input clocks back to pstate 0 -- 600MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
        JX_WR4(0x0190d000) = 0x0C12C9f0;
        JX_WR4(0x0190d000) = 0x0C12C900;
        break;
    default :
        JX_WR4(0x0190d000) = 0x0C0649f0;
        JX_WR4(0x0190d000) = 0x0C064900;
    }
    while ((JX_WR4(0x0190d080) & 0x80000000) != 0x80000000);  //: polling DDR PLL lock signal
    JX_WR4(0x0190d02c) = 0x80000000;//1<<31;       //: make DDR clock from DDR PLL
}

void SystemInit (void)
{
    smu_init();
    
    smu_ddr_freq(800);
}

uint32_t get_sysclk(void)
{
    uint32_t r_value,temp;
    sys_clk_s *value;
    temp = JX_WR4(0x0190d000);
    value = (sys_clk_s*)&temp;
    r_value = (CRYSTAL_FREQ / value->div5 / value->div2 / value->div3 * value->div4);

    if(JX_WR4(0x0190d02c)>>31){     //24M oscillator
        r_value = r_value;
    }
    else{
        r_value = r_value / 2;
    }
    return r_value;
}
