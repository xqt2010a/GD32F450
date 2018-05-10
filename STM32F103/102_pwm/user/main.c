#include "SEGGER_RTT.h"
#include "string.h"
#include "led.h"
#include "pwm.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)


void delay(uint16_t t)
{
    uint16_t i,j;
    for(i=0; i<t; i++)
        for(j=0; j<0x100; j++);
}


void main(void)
{
    uint16_t i = 0;
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    //Led_Init();
    PWM_Init();
    while(1)
    {
        //Led_Blink();
        TIM_SetCompare2(TIM3,i); //得到占空比为50%的pwm波形
        delay(100);
        i++;
        if(i>9999)
            i = 0;
    }
}
