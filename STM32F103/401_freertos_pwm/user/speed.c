#include "speed.h"



//定义左轮捕捉通道与捕获值  TIM1通道1  
#define  LEFTCAPTURECHANNEL TIM1, TIM_IT_CC1  
#define  LEFTCAPTUREVALUE TIM_GetCapture1(TIM1)  
//<span style="font-family: Arial, Helvetica, sans-serif;">TIM1通道4</span>  
#define  RIGHTCAPTURECHANNEL TIM1, TIM_IT_CC4  
#define  RIGHTCAPTUREVALUE TIM_GetCapture4(TIM1)  

#define PERIOD_BUFSIZE 20

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


void Capture_Init(void)
{
      //参数声明  
    GPIO_InitTypeDef WheelGPIO_InitStructure;  
    NVIC_InitTypeDef WheelNVIC_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef  TIM_ICInitStructure;  
      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  
      
      //速度捕获通道时基和通道配置 时钟 10000  
    TIM_TimeBaseStructure.TIM_Period = 60000;   //65535
    TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;  //采样=72000000/(psc+1)，mix=0.1ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    
      /**************左轮配置******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //上升沿
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
      
      //左轮速度捕获输入管脚配置 PA8  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM1, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM1, TIM_IT_CC1, ENABLE);   
    /**************左轮配置结束******************/  
  
    /**************右轮配置******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
        
        //右轮速度捕获输入管脚配置 PA11  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOA, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM1, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE);   
    /**************右轮配置结束******************/  
  
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
                LeftWheelPulsePeriod = ((60000 - LeftWheel1stCapture) + LeftWheel2ndCapture+1);   
            }  
            LeftPeriodBuf[LeftPeriodIndex++] = LeftWheelPulsePeriod;//记录最近的10个值  
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
            RightPeriodBuf[RightPeriodIndex++] = RightWheelPulsePeriod;//记录最近的10个值  
            if(RightPeriodIndex == PERIOD_BUFSIZE) RightPeriodIndex = 0;  
            RightWheelCaptureTime= 0;  
        }  
    }  
}  

/**------------------------------------------------------- 
  * @函数名 TIM1_IRQHandler 
  * @功能   TIM1中断处理函数，轮子测速捕获模式 
  * @参数   无 
  * @返回值 无 
***------------------------------------------------------*/  
void TIM1_CC_IRQHandler(void)  
{  
    WheelCaptureIRQ();  
}  


unsigned int GetLeftSpeed(void)  
{  
    uint16_t i;
    uint32_t sum=0, LeftValueAvg = 0;
    for(i=0; i<PERIOD_BUFSIZE; i++){
        sum += LeftPeriodBuf[i];
    }
    LeftValueAvg = sum/PERIOD_BUFSIZE;
    //PRT("\t%d\r\n",LeftValueAvg);  
    return LeftValueAvg;  
}  
  
unsigned int GetRightSpeed(void)  
{  
    uint16_t i;
    uint32_t sum = 0, RightValueAvg = 0;
    for(i=0; i<PERIOD_BUFSIZE; i++){
        sum += RightPeriodBuf[i];
    }
    RightValueAvg = sum/PERIOD_BUFSIZE;
    //PRT("\t%d\r\n",RightValueAvg);
    return RightValueAvg;  
}  


void Speed_Init(void)
{
    Capture_Init();
}

void vTask_Speed(void *p)
{
    uint16_t RightSpeedValue;
    uint16_t LeftSpeedValue; 
    while(1){
        RightSpeedValue = GetRightSpeed();
        LeftSpeedValue = GetLeftSpeed();
        PRT("R:\t %4d   L:\t %4d\r\n", RightSpeedValue, LeftSpeedValue);
        vTaskDelay(1000/portTICK_RATE_MS); 
    }
}