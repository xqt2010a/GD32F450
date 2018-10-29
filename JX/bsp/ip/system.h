#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "stdint.h"

#define CRYSTAL_FREQ    24000000

typedef struct{
    uint32_t div1:8;
    uint32_t div2:3;
    uint32_t div3:3;
    uint32_t div4:12;
    uint32_t div5:6;
}sys_clk_s;

void SystemInit (void);
uint32_t get_sysclk(void);

#endif  /* __SYSTEM_H__ */