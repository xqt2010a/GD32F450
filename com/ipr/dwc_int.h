#ifndef __DWC_INT_H__
#define __DWC_INT_H__

#include "config.h"

#define INT_BASE(x)     (SYS_INT_BASE+0x1000*(x))

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
#define IRQ_PR_N(ch,n)          (*(volatile unsigned int *)(INT_BASE(ch) + 0xe8 + 4*n))
#define IRQ_VECTOR_DEFAULT(ch)  (*(volatile unsigned int *)(INT_BASE(ch) + 0x1e8))

#endif  /* __DWC_INT_H__ */