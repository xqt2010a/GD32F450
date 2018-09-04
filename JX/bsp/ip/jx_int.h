#ifndef __JX_INT_H__
#define __JX_INT_H__

#define CPU_BASE        0xF00000

#define INT_ICPICR      (CPU_BASE+0x100)
#define INT_ICCIPMR     (CPU_BASE+0x104)
#define INT_ICCBPR      (CPU_BASE+0x108)
#define INT_ICCIAR      (CPU_BASE+0x10C)
#define INT_ICCEOIR     (CPU_BASE+0x110)
#define INT_ICCRPR      (CPU_BASE+0x114)
#define INT_ICCHPIR     (CPU_BASE+0x118)
#define INT_ICCABPR     (CPU_BASE+0x11C)
#define INT_ICCIIDR     (CPU_BASE+0x1FC)

#define INT_ICDDCR      (CPU_BASE+0x1000)
#define INT_ICDICTR     (CPU_BASE+0x1004)
#define INT_ICDIIDR     (CPU_BASE+0x1008)
#define INT_ICDISR      (CPU_BASE+0x1080)
#define INT_ICDISER     (CPU_BASE+0x1100)
#define INT_ICDICER     (CPU_BASE+0x1180)
#define INT_ICDISPR     (CPU_BASE+0x1200)
#define INT_ICDICPR     (CPU_BASE+0x1280)
#define INT_ICDABR      (CPU_BASE+0x1300)
#define INT_ICDIPR      (CPU_BASE+0x1400)
#define INT_ICDIPTR     (CPU_BASE+0x1800)
#define INT_ICDICFR     (CPU_BASE+0x1C00)
#define INT_ICDPPIS     (CPU_BASE+0x1D00)
#define INT_ICDSPIS     (CPU_BASE+0x1D04)    
#define INT_ICDSGIR     (CPU_BASE+0x1F00)
#define INT_PID         (CPU_BASE+0x1FD0)
#define INT_CID         (CPU_BASE+0x1FF0)

void IRQ_Init(void);
int IRQ_Register(unsigned int irqnum, void f(void));


#endif  /* __JX_INT_H__ */