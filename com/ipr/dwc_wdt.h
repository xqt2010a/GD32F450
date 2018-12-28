#ifndef __DWC_WDT_H__
#define __DWC_WDT_H__

#include "config.h"

#define WDT_BASE(x)     (SYS_WDT_BASE+0x1000*(x))

#define WDT_CR(ch)              (*(volatile unsigned int *)(WDT_BASE(ch) + 0x00))
#define WDT_TORR(ch)            (*(volatile unsigned int *)(WDT_BASE(ch) + 0x04))
#define WDT_CCVR(ch)            (*(volatile unsigned int *)(WDT_BASE(ch) + 0x08))
#define WDT_CRR(ch)             (*(volatile unsigned int *)(WDT_BASE(ch) + 0x0C))
#define WDT_STAT(ch)            (*(volatile unsigned int *)(WDT_BASE(ch) + 0x10))
#define WDT_EOI(ch)             (*(volatile unsigned int *)(WDT_BASE(ch) + 0x14))
#define WDT_PROT_LEVEL(ch)      (*(volatile unsigned int *)(WDT_BASE(ch) + 0x1C))
#define WDT_COMP_PARAM5(ch)     (*(volatile unsigned int *)(WDT_BASE(ch) + 0xE4))
#define WDT_COMP_PARAM4(ch)     (*(volatile unsigned int *)(WDT_BASE(ch) + 0xE8))
#define WDT_COMP_PARAM3(ch)     (*(volatile unsigned int *)(WDT_BASE(ch) + 0xEC))
#define WDT_COMP_PARAM2(ch)     (*(volatile unsigned int *)(WDT_BASE(ch) + 0xF0))
#define WDT_COMP_PARAM1(ch)     (*(volatile unsigned int *)(WDT_BASE(ch) + 0xF4))
#define WDT_COMP_VERSION(ch)    (*(volatile unsigned int *)(WDT_BASE(ch) + 0xF8))
#define WDT_COMP_TYPE(ch)       (*(volatile unsigned int *)(WDT_BASE(ch) + 0xFC))

/** 
  * @brief  WDT Init structure definition  
  */

typedef struct
{
    uint8_t ch;
    uint8_t mode;           //0:system reset 1:interrupt
    uint8_t enable;         //0:disable 1:enable
    uint32_t reload_value;
}WDT_InitTypeDef;

void dwc_wdt_set(WDT_InitTypeDef * wdt);
void dwc_wdt_mode(WDT_InitTypeDef * wdt);
void dwc_wdt_reload(WDT_InitTypeDef * wdt);
uint32_t dwc_wdt_get_value(WDT_InitTypeDef * wdt);
uint32_t dwc_wdt_it_status(WDT_InitTypeDef * wdt);
void dwc_wdt_clr_it(WDT_InitTypeDef * wdt);
void dwc_wdt_enable(WDT_InitTypeDef * wdt);

#endif  /* __DWC_WDT_H__ */