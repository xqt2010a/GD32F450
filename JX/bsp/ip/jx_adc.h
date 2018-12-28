#ifndef __JX_ADC_H__
#define __JX_ADC_H__

#include "dwc_adc.h"

typedef enum{
    ADC0  = 0,
    ADC1  = 1,
    ADC2  = 2
}ADC_n;

typedef enum{
    MODE_6BIT   = 0,
    MODE_8BIT   = 1,
    MODE_10BIT  = 2,
    MODE_11BIT  = 3
}ADC_Mode;


#endif  /* __JX_ADC_H__ */