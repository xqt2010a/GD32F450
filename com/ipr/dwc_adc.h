#ifndef __DWC_ADC_H__
#define __DWC_ADC_H__

#define ADC_BASE(x)     (SYS_ADC_BASE+0x1000*(x))

#define ADC_SCRATCH(ch)         (*(volatile unsigned int *)(ADC_BASE(ch) + 0x00))    // the receive buffer register
#define ADC_CTRL1(ch)           (*(volatile unsigned int *)(ADC_BASE(ch) + 0x04))
#define ADC_CTRL2(ch)           (*(volatile unsigned int *)(ADC_BASE(ch) + 0x08))
#define ADC_INTR_CTRL(ch)       (*(volatile unsigned int *)(ADC_BASE(ch) + 0x0c))
#define ADC_INTR_CTRL_STS(ch)   (*(volatile unsigned int *)(ADC_BASE(ch) + 0x10))

#endif  /* __DWC_ADC_H__ */