#ifndef __JX_ADC_H__
#define __JX_ADC_H__

#include "stdint.h"

#define ADC0_BASE         0x3fe00000
#define ADC1_BASE         0x3fe01000
#define ADC2_BASE         0x3fe02000

//#define ADC_BASE           ADC0_BASE

/* ------------------------  UARTn REG DEF ---------------------------------  */    // n(0,1,2)
#define SCRATCH(ADCn_BASE)              (*(volatile unsigned long*)((ADCn_BASE) + 0x00))	// the receive buffer register
#define CTRL1(ADCn_BASE)                (*(volatile unsigned long*)((ADCn_BASE) + 0x04))
#define CTRL2(ADCn_BASE)                (*(volatile unsigned long*)((ADCn_BASE) + 0x08))
#define INTR_CTRL(ADCn_BASE)            (*(volatile unsigned long*)((ADCn_BASE) + 0x0c))
#define INTR_CTRL_STS(ADCn_BASE)        (*(volatile unsigned long*)((ADCn_BASE) + 0x10))

void adc_init(uint32_t adc_n, uint8_t pin, uint8_t mode);
uint32_t adc_get(uint32_t adc_n, uint8_t pin);

#endif  /* __JX_ADC_H__ */