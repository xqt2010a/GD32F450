#ifndef __JX_INT_H__
#define __JX_INT_H__

#include "jx.h"

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

#define IRQ_INTEN_L(ch)         (*(volatile unsigned int *)(INT_BASE(ch) + 0x00))
#define IRQ_INTEN_H(ch)         (*(volatile unsigned int *)(INT_BASE(ch) + 0x04))
#define IRQ_INTMASK_L(ch)       (*(volatile unsigned int *)(INT_BASE(ch) + 0x08))
#define IRQ_INTMASK_H(ch)       (*(volatile unsigned int *)(INT_BASE(ch) + 0x0c))
#define IRQ_INTFORCE_L(ch)      (*(volatile unsigned int *)(INT_BASE(ch) + 0x10))
#define IRQ_INTFORCE_H(ch)      (*(volatile unsigned int *)(INT_BASE(ch) + 0x14))
#define IRQ_RAWSTATUS_L(ch)     (*(volatile unsigned int *)(INT_BASE(ch) + 0x18))
#define IRQ_RAWSTATUS_H(ch)     (*(volatile unsigned int *)(INT_BASE(ch) + 0x1c))
#define IRQ_STATUS_L(ch)        (*(volatile unsigned int *)(INT_BASE(ch) + 0x20))
#define IRQ_STATUS_H(ch)        (*(volatile unsigned int *)(INT_BASE(ch) + 0x24))
#define IRQ_MASKSTATUS_L(ch)    (*(volatile unsigned int *)(INT_BASE(ch) + 0x28))
#define IRQ_MASKSTATUS_H(ch)    (*(volatile unsigned int *)(INT_BASE(ch) + 0x2c))
#define IRQ_FINALSTATUS_L(ch)   (*(volatile unsigned int *)(INT_BASE(ch) + 0x30))
#define IRQ_FINALSTATUS_H(ch)   (*(volatile unsigned int *)(INT_BASE(ch) + 0x34))
#define IRQ_VECTOR(ch)          (*(volatile unsigned int *)(INT_BASE(ch) + 0x38))
#define IRQ_VECTOR_N(ch,n)      (*(volatile unsigned int *)(INT_BASE(ch) + 0x40 + 8*n))

#define FIQ_INTEN(ch)           (*(volatile unsigned int *)(INT_BASE(ch) + 0xc0))
#define FIQ_INTMASK(ch)         (*(volatile unsigned int *)(INT_BASE(ch) + 0xc4))
#define FIQ_INTFORCE(ch)        (*(volatile unsigned int *)(INT_BASE(ch) + 0xc8))
#define FIQ_RAWSTATUS(ch)       (*(volatile unsigned int *)(INT_BASE(ch) + 0xcc))
#define FIQ_STATUS(ch)          (*(volatile unsigned int *)(INT_BASE(ch) + 0xd0))
#define FIQ_FINALSTATUS(ch)     (*(volatile unsigned int *)(INT_BASE(ch) + 0xd4))
#define IRQ_PLEVEL(ch)          (*(volatile unsigned int *)(INT_BASE(ch) + 0xd8))
#define ICTL_VERSION_ID(ch)     (*(volatile unsigned int *)(INT_BASE(ch) + 0xe0))
#define IRQ_PR_N(ch)            (*(volatile unsigned int *)(INT_BASE(ch) + 0xe8 + 4*n))
#define IRQ_VECTOR_DEFAULT(ch)  (*(volatile unsigned int *)(INT_BASE(ch) + 0x1e8))

void IRQ_Init(void);
void IRQ_SetEnable(void);
int IRQ_Register(unsigned int irqnum, void f(void));


#endif  /* __JX_INT_H__ */