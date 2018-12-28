#include "dwc_adc.h"

void dwc_adc_init(ADC_InitTypeDef * adc)
{
    uint32_t temp;
    ADC_SCRATCH(adc->ch) = adc->sample_cnt;      //采样间隔
    
    temp = ADC_CTRL1(adc->ch);
    temp |= (uint32_t)(1<<31); 
    ADC_CTRL1(adc->ch) = temp;        //enadc
    ADC_CTRL2(adc->ch) = 1;
    asm("nop");
    ADC_CTRL2(adc->ch) = 0;           //reset
    temp = (uint32_t)((1<<0)|(1<<1)|(0<<2)|(adc->mode<<8)|(adc->pin<<12)|(3<<19)|(1<<31));   //11bit mode; 8次平均;
    ADC_CTRL1(adc->ch) = temp;
    temp = ADC_CTRL1(adc->ch);
    temp |= (1<<10);    
    ADC_CTRL1(adc->ch) = temp;        //enable
    ADC_INTR_CTRL(adc->ch) = adc->int_mask;       //enable interrupt
}

uint32_t dwc_adc_get(ADC_InitTypeDef * adc)
{
    uint32_t temp;//, r_value;
    
    do{
        temp = ADC_INTR_CTRL_STS(adc->ch);
    }
    while((temp & 0x80000000) == 0);
    //r_value = temp & 0xFF;
    ADC_INTR_CTRL_STS(adc->ch) = (uint32_t)(1<<31); //clear interrupt
    return (temp & 0xFFF);
}
