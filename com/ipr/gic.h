#ifndef __GIC_H__
#define __GIC_H__

#define GICC_BASE       (SYS_GIC_BASE + 0x100)
#define GICD_BASE       (SYS_GIC_BASE + 0x1000)

#define GICC_CTLR       (*(volatile unsigned int *)(GICC_BASE + 0x0))
#define GICC_PMR        (*(volatile unsigned int *)(GICC_BASE + 0x4))
#define GICC_IAR        (*(volatile unsigned int *)(GICC_BASE + 0xC))
#define GICC_EOIR       (*(volatile unsigned int *)(GICC_BASE + 0x10))

#define GICD_CTRL           (*(volatile unsigned int *)(GICD_BASE + 0x0))
#define GICD_TYPER          (*(volatile unsigned int *)(GICD_BASE + 0x4))
#define GICD_GROUP(n)       (*(volatile unsigned int *)(GICD_BASE + 0x80 + 4*(n)))
#define GICD_ISENABLER(n)   (*(volatile unsigned int *)(GICD_BASE + 0x100 + 4*(n)))
#define GICD_ISPENDR(n)     (*(volatile unsigned int *)(GICD_BASE + 0x200 + 4*(n)))
#define GICD_ICPENDR(n)     (*(volatile unsigned int *)(GICD_BASE + 0x280 + 4*(n)))
#define GICD_IPRIORITYR(n)  (*(volatile unsigned int *)(GICD_BASE + 0x400 + 4*(n)))
#define GICD_ITARGETSR(n)   (*(volatile unsigned int *)(GICD_BASE + 0x800 + 4*(n)))
#define GICD_ICFGR(n)       (*(volatile unsigned int *)(GICD_BASE + 0xC00 + 4*(n)))

#endif  /* __GIC_H__ */