#include "jx.h"

//extern int __ICFEDIT_region_ROM_start__;

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

static void smu_ddr_freq(unsigned int freq_MHZ)
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
    //smu_init();
    
    smu_ddr_freq(800);
}

//void LowLevelInit(void)
//{   
//        //HWMEM_WORD(0xf00110UL) = 0;
//    /* Remap */
//	/* Set V=0 in CP15 SCTLR register - for VBAR to point to vector */
//	 __asm("mrc	p15, 0, r0, c1, c0, 0"); //	@ Read CP15 SCTLR Register
//         __asm("bic	r0, r0, #0x00002000"); //	@ clear bits 13 (--V-)         
//	 __asm("mcr	p15, 0, r0, c1, c0, 0"); //	@ Write CP15 SCTLR Register
//	/* Set vector address in CP15 VBAR register */
//	 //__asm("ldr	r0, =0x80000"); //
//     __asm("ldr r0, =0x80000");
//	 __asm("mcr	p15, 0, r0, c12, c0, 0"); //	@Set VBAR
//}