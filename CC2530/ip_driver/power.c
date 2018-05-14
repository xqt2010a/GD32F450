#include "power.h"

void PWR_GpioInit(void)
{
    P1SEL &= ~0X03; //P1_0,P1_1�ӿ���Ϊͨ�ýӿ�ģʽ  
    P1DIR |= 0X03;  //P1_0,P1_1�ӿ���Ϊ���ģʽ  
    
    //P0DIR |= 0x40;  //P0_6 ���
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

//������ʱ��
void PWR_MainClock(unsigned char source)
{  
    if(source == RCOSC)  
    {    
        CLKCONCMD |= 0X40;          //ѡ��16MHZ RCOSCΪϵͳʱ��Դ    
        while(!(CLKCONSTA & 0X40)); //�ȴ�ʱ���ȶ�
    }else
    {    
        CLKCONCMD &= ~0X40;         //ѡ��32MHZ XOSCΪϵͳʱ��Դ    
        while(CLKCONSTA & 0X40);    //�ȴ�ʱ���ȶ�
    }
}

//����ϵͳ����ʱ��
void PWR_LowClock(unsigned char source)
{  
    if(source == RCOSC)   
        CLKCONCMD |= 0X80;         //ѡ��32KHZ RCOSCΪ����ʱ��Դ    
    else    
        CLKCONCMD &= ~0X80;        //ѡ��32KHZ XOSCΪ����ʱ��Դ
}


