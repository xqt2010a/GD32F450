#include "system.h"
#include "jx_uart.h"
#include "jx_adc.h"
#include "stdio.h"

#define ADC_BASE           ADC0_BASE
#define JX_W4(x)    (*(unsigned int *)(x))

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init1(void)
{
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08104) = 0xFFFFFFFF;
    JX_W4(0x0190d100) = 0xFFFFFFFF;
    JX_W4(0x0190d104) = 0xFFFFFFFF;
    JX_W4(0x0190d108) = 0xFFFFFFFF;
    JX_W4(0x0190d10c) = 0xFFFFFFFF;
    JX_W4(0x0190d110) = 0xFFFFFFFF;
    JX_W4(0x0190d114) = 0xFFFFFFFF;
    JX_W4(0x0190d120) = 0xFFFFFFFF;
    JX_W4(0x0190d124) = 0xFFFFFFFF;
    
    /* Delay for somewhile to wait reset de-assertion to be stable. */
    udelay(10000);
}

void main(void)
{
    uint32_t sysclk;
    uint32_t voltage[24];//,i;
    
    smu_init1();
    adc_init(ADC_BASE, 0);
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\n");
//    for(i=0; i<8; i++)
//        adc_init(ADC0_BASE, i);
//    for(i=0; i<8; i++)
//        adc_init(ADC1_BASE, i);
//    for(i=0; i<8; i++)
//        adc_init(ADC2_BASE, i);
    adc_init(ADC_BASE, 0);
    while(1){
        //adc_init(ADC_BASE, 0);
        
//        for(i=0; i<8; i++)
//            voltage[i] = adc_get(ADC0_BASE, 0);
//        for(i=8; i<16; i++)
//            voltage[i] = adc_get(ADC1_BASE, 0);
//        for(i=16; i<24; i++)
//            voltage[i] = adc_get(ADC2_BASE, 0);
//        for(i=0; i<24; i++)
//            printf(" %3x",voltage[i]);
        voltage[0] = adc_get(ADC0_BASE, 0);
        printf("reg:%3x %3x",CTRL1(ADC0_BASE), voltage[0]);
        printf("\r\n");
        udelay(100000);
    }
}