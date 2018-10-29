#include "jx_adc.h"


void adc_init(uint32_t adc_n, uint8_t pin)
{
    uint32_t temp;
    SCRATCH(adc_n) = 0x0a;      //采样间隔
    
    temp = CTRL1(adc_n);
    temp |= (uint32_t)(1<<31); 
    CTRL1(adc_n) = temp;        //enadc
    CTRL2(adc_n) = 1;
    asm("nop");
    CTRL2(adc_n) = 0;           //reset
    temp = (uint32_t)((1<<0)|(1<<1)|(0<<2)|(3<<8)|(pin<<12)|(3<<19)|(1<<31));   //11bit mode; 8次平均;
    CTRL1(adc_n) = temp;
    temp = CTRL1(adc_n);
    temp |= (1<<10);    
    CTRL1(adc_n) = temp;        //enable
    //INTR_CTRL(adc_n) = 1;       //enable interrupt
}

uint32_t adc_get(uint32_t adc_n, uint8_t pin)
{
    uint32_t temp;//, r_value;
    
    do{
        temp = INTR_CTRL_STS(adc_n);
    }
    while((temp & 0x80000000) == 0);
    //r_value = temp & 0xFF;
    INTR_CTRL_STS(adc_n) = (uint32_t)(1<<31); //clear interrupt
    return (temp & 0xFFF);
}
