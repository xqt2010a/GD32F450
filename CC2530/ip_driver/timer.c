#include "timer.h"

void Timer1_Init(unsigned char value)  //�������value/125 ms
{
    T1CTL = 0x0C;         // @DIV��Ƶϵ�� 128 @MODE��ͣ����
    T1CCTL0 = 0x44;       // @IMͨ��0�ж�ʹ�� @MODE�Ƚ�ƥ��ģʽ
    T1STAT = 0x00;        // ��������жϱ�־

    T1IE = 1;             // IEN1@BIT1 ʹ�ܶ�ʱ��1�ж�

    T1CC0L = value;         // �������Ϊvalue/125 ms
    T1CC0H = 0;
}

void Timer1_Disbale(void)
{
    T1CTL &= ~( 1<< 1);   // �ָ�Ϊֹͣģʽ
}

void Timer1_Enable(void)
{
    T1CTL |= ( 1 << 1 );  // �ı�ģʽΪ�Ƚ�ƥ��ģʽ MODE = 0x10;
    T1STAT = 0x00;        // ����жϱ�־λ
    T1CNTH = 0;           // ���¿�ʼ����
    T1CNTL = 0;
}

//��ʼ��˯�߶�ʱ��
void TIMER_SleepInit(void)
{  
    ST2 = 0X00;  
    ST1 = 0X00;  
    ST0 = 0X00; //���ü���ֵ  
    EA = 1;     //���ж�  
    STIE = 1;   //ʹ��˯�߶�ʱ���ж�  
    STIF = 0;   //���˯�߶�ʱ���жϱ�־
}

//����˯��ʱ��
void TIMER_SleepTime(unsigned int sec)
{  
    unsigned long sleeptimer = 0;  
    //��ST2:ST1:ST0��ֵ��sleeptimer  
    sleeptimer |= (unsigned long)ST0;  
    sleeptimer |= ((unsigned long)ST1 << 8);  
    sleeptimer |= ((unsigned long)ST2 << 16);
    sleeptimer += ((unsigned long)sec * (unsigned long)32768);  //����Ƶ��Ϊ32.768KHZ,��ÿ�붨ʱ������32768���������ʱ��Ϊ32K��    
  
    //�Ѽ�N��ļ���ֵ����ST2:ST1:ST0  
    ST2 = (unsigned char)(sleeptimer >> 16);  
    ST1 = (unsigned char)(sleeptimer >> 8);  
    ST0 = (unsigned char)sleeptimer;
}
