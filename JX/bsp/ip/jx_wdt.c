#include "jx_wdt.h"

#define CRR_KEY_RELOAD 0x76

void IWDG_SetReload(uint32_t CounterValue)
{
    uint32_t value;
    value = WDT_TORR;
    WDT_TORR = (value & (~0x0000000f)) | CounterValue;
}

void IWDG_Enable(uint8_t mode)
{
    uint32_t temp_reg;
    temp_reg = WDT_CR & (~0xf);
    mode = mode ? (1):(0);
    temp_reg = temp_reg | (mode<<1) | (7<<2);
    WDT_CR = temp_reg;
    WDT_CR = temp_reg | (1);
}

void IWDG_ReloadCounter(void)
{
    WDT_CRR = CRR_KEY_RELOAD;
}

uint32_t IWDG_GetCurrentValue(void)
{
    return WDT_CCVR;
}

uint32_t IWDG_GetITStatus(void)
{
    return WDT_STAT;
}


void IWDG_ClearITPendingBit(void)
{
    volatile int temp;
    temp = WDT_EOI;
}

void IWDG_Disable(void)
{
    uint32_t value;
    value = WDT_CR;
    value = value & (~0x00000001);
    WDT_CR = value;
}