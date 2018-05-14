#include "timer.h"

void Timer1_Init(unsigned char value)  //溢出周期value/125 ms
{
    T1CTL = 0x0C;         // @DIV分频系数 128 @MODE暂停运行
    T1CCTL0 = 0x44;       // @IM通道0中断使能 @MODE比较匹配模式
    T1STAT = 0x00;        // 清除所有中断标志

    T1IE = 1;             // IEN1@BIT1 使能定时器1中断

    T1CC0L = value;         // 溢出周期为value/125 ms
    T1CC0H = 0;
}

void Timer1_Disbale(void)
{
    T1CTL &= ~( 1<< 1);   // 恢复为停止模式
}

void Timer1_Enable(void)
{
    T1CTL |= ( 1 << 1 );  // 改变模式为比较匹配模式 MODE = 0x10;
    T1STAT = 0x00;        // 清除中断标志位
    T1CNTH = 0;           // 重新开始计数
    T1CNTL = 0;
}

//初始化睡眠定时器
void TIMER_SleepInit(void)
{  
    ST2 = 0X00;  
    ST1 = 0X00;  
    ST0 = 0X00; //设置计数值  
    EA = 1;     //开中断  
    STIE = 1;   //使能睡眠定时器中断  
    STIF = 0;   //清除睡眠定时器中断标志
}

//设置睡眠时间
void TIMER_SleepTime(unsigned int sec)
{  
    unsigned long sleeptimer = 0;  
    //把ST2:ST1:ST0赋值给sleeptimer  
    sleeptimer |= (unsigned long)ST0;  
    sleeptimer |= ((unsigned long)ST1 << 8);  
    sleeptimer |= ((unsigned long)ST2 << 16);
    sleeptimer += ((unsigned long)sec * (unsigned long)32768);  //低速频率为32.768KHZ,故每秒定时器计数32768次如果低速时钟为32K呢    
  
    //把加N秒的计数值赋给ST2:ST1:ST0  
    ST2 = (unsigned char)(sleeptimer >> 16);  
    ST1 = (unsigned char)(sleeptimer >> 8);  
    ST0 = (unsigned char)sleeptimer;
}
