#ifndef __DWC_WDT_H__
#define __DWC_WDT_H__

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

#endif  /* __DWC_WDT_H__ */