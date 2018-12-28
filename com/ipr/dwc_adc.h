#ifndef __DWC_ADC_H__
#define __DWC_ADC_H__

#include "config.h"

#define ADC_BASE(x)     (SYS_ADC_BASE+0x1000*(x))

#define ADC_SCRATCH(ch)         (*(volatile unsigned int *)(ADC_BASE(ch) + 0x00))    // the receive buffer register
#define ADC_CTRL1(ch)           (*(volatile unsigned int *)(ADC_BASE(ch) + 0x04))
#define ADC_CTRL2(ch)           (*(volatile unsigned int *)(ADC_BASE(ch) + 0x08))
#define ADC_INTR_CTRL(ch)       (*(volatile unsigned int *)(ADC_BASE(ch) + 0x0c))
#define ADC_INTR_CTRL_STS(ch)   (*(volatile unsigned int *)(ADC_BASE(ch) + 0x10))

/**
  * @brief  ADC Init structure definition
  */
  
typedef struct
{
    uint8_t ch;
    uint8_t pin;
    uint8_t mode;       //0:6bit 1:8bit 2:10bit 3:11bit
    uint32_t int_mask;    /* see datasheet */
    uint32_t sample_cnt;    //sample count[31:0]
    uint32_t clk;
} ADC_InitTypeDef;

void dwc_adc_init(ADC_InitTypeDef * adc);
uint32_t dwc_adc_get(ADC_InitTypeDef * adc);

#endif  /* __DWC_ADC_H__ */