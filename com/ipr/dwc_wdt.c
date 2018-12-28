#include "dwc_wdt.h"

#define CRR_KEY_RELOAD 0x76

void dwc_wdt_set(WDT_InitTypeDef * wdt)
{
    uint32_t value;
    value = WDT_TORR(wdt->ch);
    WDT_TORR(wdt->ch) = (value & (~0x0000000f)) | wdt->reload_value;
}

void dwc_wdt_mode(WDT_InitTypeDef * wdt)
{
    uint32_t temp_reg;
    temp_reg = WDT_CR(wdt->ch) & (~0xf);
    wdt->mode = wdt->mode ? (1):(0);
    temp_reg = temp_reg | (wdt->mode<<1) | (7<<2);
    WDT_CR(wdt->ch) = temp_reg;
}

void dwc_wdt_reload(WDT_InitTypeDef * wdt)
{
    WDT_CRR(wdt->ch) = CRR_KEY_RELOAD;
}

uint32_t dwc_wdt_get_value(WDT_InitTypeDef * wdt)
{
    return WDT_CCVR(wdt->ch);
}

uint32_t dwc_wdt_it_status(WDT_InitTypeDef * wdt)
{
    return WDT_STAT(wdt->ch);
}

//清看门狗中断标志
void dwc_wdt_clr_it(WDT_InitTypeDef * wdt)
{
    volatile int temp;
    temp = WDT_EOI(wdt->ch);
}

void dwc_wdt_enable(WDT_InitTypeDef * wdt)
{
    uint32_t value;
    value = WDT_CR(wdt->ch);
    wdt->enable = wdt->enable ? 1 : 0;
    value = value & (~0x00000001);
    WDT_CR(wdt->ch) = value | wdt->enable;
}