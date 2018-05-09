#include "speed.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"

//�������ֲ�׽ͨ���벶��ֵ  TIM1ͨ��1  
#define  LEFTCAPTURECHANNEL TIM1, TIM_IT_CC1  
#define  LEFTCAPTUREVALUE TIM_GetCapture1(TIM1)  
//<span style="font-family: Arial, Helvetica, sans-serif;">TIM1ͨ��4</span>  
#define  RIGHTCAPTURECHANNEL TIM1, TIM_IT_CC4  
#define  RIGHTCAPTUREVALUE TIM_GetCapture4(TIM1)  

#define PERIOD_BUFSIZE 30

uint16_t speed_contiue;
uint16_t RightWheelPulsePeriod;

uint16_t LeftPeriodIndex = 0;
uint16_t RightPeriodIndex = 0;

uint16_t LeftWheelPulsePeriod = 0;
uint16_t LRightWheelPulsePeriod = 0;


uint16_t LeftWheel1stCapture = 0;
uint16_t RightWheel1stCapture = 0;

uint16_t LeftWheel2ndCapture = 0;
uint16_t RightWheel2ndCapture = 0;

uint16_t LeftWheelCaptureTime = 0;
uint16_t RightWheelCaptureTime = 0;




uint16_t RightPeriodBuf[PERIOD_BUFSIZE];
uint16_t LeftPeriodBuf[PERIOD_BUFSIZE];

void MeaSpeedInit(void)
{
      //��������  
    GPIO_InitTypeDef WheelGPIO_InitStructure;  
    NVIC_InitTypeDef WheelNVIC_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef  TIM_ICInitStructure;  
      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  
      
      //�ٶȲ���ͨ��ʱ����ͨ������ ʱ�� 10000  
    TIM_TimeBaseStructure.TIM_Period = 65535;   
    TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    
      /**************��������******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
      
      //�����ٶȲ�������ܽ����� PA8  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM1, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);   
    /**************�������ý���******************/  
  
    /**************��������******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
        
        //�����ٶȲ�������ܽ����� PA11  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM1, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);   
    /**************�������ý���******************/  
  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
    WheelNVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;  
    WheelNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    WheelNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    WheelNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&WheelNVIC_InitStructure);  
    
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  
    TIM_Cmd(TIM1, ENABLE);  
}

void WheelCaptureIRQ(void)  
{  
    if(TIM_GetITStatus(LEFTCAPTURECHANNEL) == SET)   
    {  
        speed_contiue=100;  
        TIM_ClearITPendingBit(LEFTCAPTURECHANNEL);  
        if(LeftWheelCaptureTime == 0)  
        {  
            LeftWheel1stCapture = LEFTCAPTUREVALUE;  
            LeftWheelCaptureTime = 1;  
        }  
        else if(LeftWheelCaptureTime == 1)  
        {  
            LeftWheel2ndCapture = LEFTCAPTUREVALUE;   
            if (LeftWheel2ndCapture > LeftWheel1stCapture)  
            {  
                LeftWheelPulsePeriod = (LeftWheel2ndCapture - LeftWheel1stCapture);   
            }  
            else  
            {  
                LeftWheelPulsePeriod = ((0xFFFF - LeftWheel1stCapture) + LeftWheel2ndCapture);   
            }  
                LeftPeriodBuf[LeftPeriodIndex++] = LeftWheelPulsePeriod;//��¼�����10��ֵ  
            if(LeftPeriodIndex == PERIOD_BUFSIZE)   
                LeftPeriodIndex = 0;  
            LeftWheelCaptureTime= 0;  
        }  
    }  
      
    if(TIM_GetITStatus(RIGHTCAPTURECHANNEL) == SET)   
    {  
        speed_contiue=100;  
        TIM_ClearITPendingBit(RIGHTCAPTURECHANNEL);  
        if(RightWheelCaptureTime == 0)  
        {  
            RightWheel1stCapture = RIGHTCAPTUREVALUE;  
            RightWheelCaptureTime = 1;  
        }  
        else if(RightWheelCaptureTime == 1)  
        {  
            RightWheel2ndCapture = RIGHTCAPTUREVALUE;   
            if (RightWheel2ndCapture > RightWheel1stCapture)  
            {  
                RightWheelPulsePeriod = (RightWheel2ndCapture - RightWheel1stCapture);   
            }  
            else  
            {  
                RightWheelPulsePeriod = ((0xFFFF - RightWheel1stCapture) + RightWheel2ndCapture);   
            }  
            RightPeriodBuf[RightPeriodIndex++] = RightWheelPulsePeriod;//��¼�����10��ֵ  
            if(RightPeriodIndex == PERIOD_BUFSIZE) RightPeriodIndex = 0;  
            RightWheelCaptureTime= 0;  
        }  
    }  
}  

/**------------------------------------------------------- 
  * @������ TIM1_IRQHandler 
  * @����   TIM1�жϴ����������Ӳ��ٲ���ģʽ 
  * @����   �� 
  * @����ֵ �� 
***------------------------------------------------------*/  
void TIM1_CC_IRQHandler(void)  
{  
    WheelCaptureIRQ();  
}  


unsigned int GetLeftSpeed(void)  
{  
    int index,intex_temp=0;  
    uint16_t LeftPeriodBufTemp[PERIOD_BUFSIZE];  
    u8 max_index=0,min_index=0;  
    uint16_t LeftValueTemp=0;  
    unsigned int LeftValueAvg=0;//ƽ��ֵ  
    u8 index_total=0;  
    //���洢�����ݰ�˳���ȡ�����ػ���  
    for(index = LeftPeriodIndex;index < PERIOD_BUFSIZE ;index++)  
    {  
        LeftPeriodBufTemp[intex_temp++] = LeftPeriodBuf[index];  
    }  
    for(index = 0;index < LeftPeriodIndex ;index++)  
    {  
        LeftPeriodBufTemp[intex_temp++] = LeftPeriodBuf[index];  
    }     
      
    //ȡ�����ֵ������  
    LeftValueTemp = LeftPeriodBufTemp[0];  
    for(index = 0;index < PERIOD_BUFSIZE-1; index++)  
    {  
        if(LeftValueTemp < LeftPeriodBufTemp[index+1])  
        {  
            max_index = index+1;  
            LeftValueTemp = LeftPeriodBufTemp[index+1];  
        }  
    }  
    //ȡ����Сֵ������  
    LeftValueTemp = LeftPeriodBufTemp[0];  
    for(index = 0;index < PERIOD_BUFSIZE-1; index++)  
    {  
        if(LeftValueTemp > LeftPeriodBufTemp[index+1])  
        {  
            min_index = index+1;  
            LeftValueTemp = LeftPeriodBufTemp[index+1];  
        }  
    }  
      
    //ȥ�����ֵ����Сֵ ���Ȩƽ��ֵ  
    for(index = 0;index < PERIOD_BUFSIZE;index++)  
    {  
        if((index != min_index) && (index != max_index))  
        {  
            LeftValueAvg += LeftPeriodBufTemp[index]*(index+1);  
            index_total += index+1;  
        }  
    }  
    LeftValueAvg = LeftValueAvg/index_total;//ȡƽ��ֵ  
      
    for(index = 0;index < PERIOD_BUFSIZE;index++)  
    {  
        printf("%d\t",LeftPeriodBufTemp[index]);  
    }     
      
    if(LeftValueAvg == 0) return 0;  
    LeftValueAvg = 10445/LeftValueAvg;  
    printf("\t%d\r\n",LeftValueAvg);  
    return LeftValueAvg;  
}  
  
unsigned int GetRightSpeed(void)  
{  
    int index,intex_temp=0;  
    uint16_t RightPeriodBufTemp[PERIOD_BUFSIZE];  
    u8 max_index=0,min_index=0;  
    uint16_t RightValueTemp=0;  
    unsigned int RightValueAvg=0;//ƽ��ֵ  
    u8 index_total=0;  
      
    //���洢�����ݰ�˳���ȡ�����ػ���  
    for(index = RightPeriodIndex;index < PERIOD_BUFSIZE ;index++)  
    {  
        RightPeriodBufTemp[intex_temp++] = RightPeriodBuf[index];  
    }  
    for(index = 0;index < RightPeriodIndex ;index++)  
    {  
        RightPeriodBufTemp[intex_temp++] = RightPeriodBuf[index];  
    }     
      
    //ȡ�����ֵ������  
    RightValueTemp = RightPeriodBufTemp[0];  
    for(index = 0;index < PERIOD_BUFSIZE-1; index++)  
    {  
        if(RightValueTemp < RightPeriodBufTemp[index+1])  
        {  
            max_index = index+1;  
            RightValueTemp = RightPeriodBufTemp[index+1];  
        }  
    }  
    //ȡ����Сֵ������  
    RightValueTemp = RightPeriodBufTemp[0];  
    for(index = 0;index < PERIOD_BUFSIZE-1; index++)  
    {  
        if(RightValueTemp > RightPeriodBufTemp[index+1])  
        {  
            min_index = index+1;  
            RightValueTemp = RightPeriodBufTemp[index+1];  
        }  
    }  
      
    //ȥ�����ֵ����Сֵ ���Ȩƽ��ֵ  
    for(index = 0;index < PERIOD_BUFSIZE;index++)  
    {  
        if((index != min_index) && (index != max_index))  
        {  
            RightValueAvg += RightPeriodBufTemp[index]*(index+1);  
            index_total += index+1;  
        }  
    }  
    RightValueAvg = RightValueAvg/index_total;//ȡƽ��ֵ  
    if(RightValueAvg == 0) return 0;  
    RightValueAvg = 10445/RightValueAvg;  
    return RightValueAvg;  
}  