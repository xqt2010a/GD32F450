#ifndef __JX_WDT_H__
#define __JX_WDT_H__

#include "jx.h"

#define WDT_BASE    0x01908000

#define WDT_CR              JX_WR4(WDT_BASE + 0x00)
#define WDT_TORR            JX_WR4(WDT_BASE + 0x04)
#define WDT_CCVR            JX_WR4(WDT_BASE + 0x08)
#define WDT_CRR             JX_WR4(WDT_BASE + 0x0C)
#define WDT_STAT            JX_WR4(WDT_BASE + 0x10)
#define WDT_EOI             JX_WR4(WDT_BASE + 0x14)
#define WDT_PROT_LEVEL      JX_WR4(WDT_BASE + 0x1C)
#define WDT_COMP_PARAM5     JX_WR4(WDT_BASE + 0xE4)
#define WDT_COMP_PARAM4     JX_WR4(WDT_BASE + 0xE8)
#define WDT_COMP_PARAM3     JX_WR4(WDT_BASE + 0xEC)
#define WDT_COMP_PARAM2     JX_WR4(WDT_BASE + 0xF0)
#define WDT_COMP_PARAM1     JX_WR4(WDT_BASE + 0xF4)
#define WDT_COMP_VERSION    JX_WR4(WDT_BASE + 0xF8)
#define WDT_COMP_TYPE       JX_WR4(WDT_BASE + 0xFC)

void IWDG_SetReload(uint32_t CounterValue);
void IWDG_Enable(uint8_t mode);
void IWDG_ReloadCounter(void);
uint32_t IWDG_GetCurrentValue(void);
uint32_t IWDG_GetITStatus(void);
void IWDG_ClearITPendingBit(void);
void IWDG_Disable(void);

#endif  /* __JX_WDT_H__ */