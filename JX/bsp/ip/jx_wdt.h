#ifndef __JX_WDT_H__
#define __JX_WDT_H__

#include "dwc_wdt.h"

typedef enum{
    WDT0  = 0
}WDT_n;

typedef enum{
    CLK_64K  = 0x0,
    CLK_128K = 0x1,
    CLK_256K = 0x2,
    CLK_512L = 0x3,
    CLK_1M   = 0x4,
    CLK_2M   = 0x5,
    CLK_4M   = 0x6,
    CLK_8M   = 0x7,
    CLK_16M  = 0x8,
    CLK_32M  = 0x9,
    CLK_64M  = 0xa,
    CLK_128M = 0xb,
    CLK_256M = 0xc,
    CLK_512M = 0xd,
    CLK_1G   = 0xe,
    CLK_2G   = 0xf
}TimeOut_Clk;

void wdt_init(WDT_InitTypeDef * wdt);

#endif  /* __JX_WDT_H__ */