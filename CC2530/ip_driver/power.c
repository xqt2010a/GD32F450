#include "power.h"

void PWR_GpioInit(void)
{
    P1SEL &= ~0X03; //P1_0,P1_1接口设为通用接口模式  
    P1DIR |= 0X03;  //P1_0,P1_1接口设为输出模式  
    
    //P0DIR |= 0x40;  //P0_6 输出
    //P0_6 = 1;
}

void PWR_Mode(unsigned char mode)
{
    unsigned char i;
    if(mode<4)
    {
        SLEEPCMD &= ~0X03;
        SLEEPCMD |= mode;
        for(i=0;i<4;i++);      
        PCON = 1;
        asm("NOP");
    }
    else
        PCON = 0;
    asm("NOP");
}

//设置主时钟
void PWR_MainClock(unsigned char source)
{  
    if(source == RCOSC)  
    {    
        CLKCONCMD |= 0X40;          //选择16MHZ RCOSC为系统时钟源    
        while(!(CLKCONSTA & 0X40)); //等待时钟稳定
    }else
    {    
        CLKCONCMD &= ~0X40;         //选择32MHZ XOSC为系统时钟源    
        while(CLKCONSTA & 0X40);    //等待时钟稳定
    }
}

//设置系统低速时钟
void PWR_LowClock(unsigned char source)
{  
    if(source == RCOSC)   
        CLKCONCMD |= 0X80;         //选择32KHZ RCOSC为低速时钟源    
    else    
        CLKCONCMD &= ~0X80;        //选择32KHZ XOSC为低速时钟源
}


