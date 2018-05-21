#include "speed.h"
#include "string.h"


//定义左轮捕捉通道与捕获值  TIM2通道3 
#define  LEFTCAPTURECHANNEL TIM2, TIM_IT_CC3  
#define  LEFTCAPTUREVALUE TIM_GetCapture3(TIM2)  
//<span style="font-family: Arial, Helvetica, sans-serif;">TIM1通道4</span>  
#define  RIGHTCAPTURECHANNEL TIM2, TIM_IT_CC4  
#define  RIGHTCAPTUREVALUE TIM_GetCapture4(TIM2)  

#define PERIOD_BUFSIZE      1

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

#if(DEBUG_COUNT)
uint32_t Right_Count = 0;
uint32_t Left_Count = 0;
#endif  /* DEBUG_COUNT */

void Capture_Init(void)
{
      //参数声明  
    GPIO_InitTypeDef WheelGPIO_InitStructure;  
    NVIC_InitTypeDef WheelNVIC_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef  TIM_ICInitStructure;  
      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//使能GPIO外设和AFIO复用功能模块时钟
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); //选择Timer2全部重映像    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
      
      //速度捕获通道时基和通道配置 时钟 10000  
    TIM_TimeBaseStructure.TIM_Period = 60000;   //65535
    TIM_TimeBaseStructure.TIM_Prescaler = 720-1;  //采样=72000000/(psc+1)，mix=0.01ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    
      /**************左轮配置******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //上升沿
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
      
      //左轮速度捕获输入管脚配置 PB10
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM2, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);   
    /**************左轮配置结束******************/  
  
    /**************右轮配置******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
        
        //右轮速度捕获输入管脚配置 PB11  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM2, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);   
    /**************右轮配置结束******************/  
  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  
    WheelNVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
    WheelNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    WheelNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    WheelNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&WheelNVIC_InitStructure);  
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  
    TIM_Cmd(TIM2, ENABLE);  
}

void WheelCaptureIRQ(void)  
{  
      
      
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
            if (RightWheel2ndCapture >= RightWheel1stCapture)  
            {  
                RightWheelPulsePeriod = (RightWheel2ndCapture - RightWheel1stCapture);   
            }  
            else  
            {  
                RightWheelPulsePeriod = ((60000 - RightWheel1stCapture) + RightWheel2ndCapture);   
            }  
            RightPeriodBuf[RightPeriodIndex++] = RightWheelPulsePeriod;//记录最近的10个值  
#if(DEBUG_COUNT)            
            Right_Count++;
#endif  /* DEBUG_COUNT */
            
            if(RightPeriodIndex == PERIOD_BUFSIZE) 
                RightPeriodIndex = 0;  
            RightWheelCaptureTime= 0;  
        }  
    }  
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
#if(DEBUG_COUNT)            
            Left_Count++;
#endif  /* DEBUG_COUNT */
            
            if(LeftPeriodIndex == PERIOD_BUFSIZE)   
                LeftPeriodIndex = 0;  
            LeftWheelCaptureTime= 0;  
        }  
    }
}  

/**------------------------------------------------------- 
  * @函数名 TIM1_IRQHandler 
  * @功能   TIM1中断处理函数，轮子测速捕获模式 
  * @参数   无 
  * @返回值 无 
***------------------------------------------------------*/  
void TIM2_IRQHandler(void)  
{  
    WheelCaptureIRQ();  
}  


unsigned int GetLeftCount(void)  
{  
    uint16_t i;
    uint32_t sum=0, LeftValueAvg = 0;
    for(i=0; i<PERIOD_BUFSIZE; i++){
        sum += LeftPeriodBuf[i];
        //LeftPeriodBuf[i] = 0;
    }
    LeftValueAvg = sum/PERIOD_BUFSIZE;
    for(i=0; i<PERIOD_BUFSIZE; i++){
        LeftPeriodBuf[i] = 0;
    }
    //PRT("\t%d\r\n",LeftValueAvg);  
    return LeftValueAvg;  
}  
  
unsigned int GetRightCount(void)  
{  
    uint16_t i;
    uint32_t sum = 0, RightValueAvg = 0;
    for(i=0; i<PERIOD_BUFSIZE; i++){
        sum += RightPeriodBuf[i];
        //RightPeriodBuf[i] = 0;
    }
    RightValueAvg = sum/PERIOD_BUFSIZE;  
    for(i=0; i<PERIOD_BUFSIZE; i++){
        RightPeriodBuf[i] = 0;
    }
    //PRT("\t%d\r\n",RightValueAvg);
    return RightValueAvg;  
}  


void Speed_Init(void)
{
    Capture_Init();
}

void vTask_Speed(void *p)
{
    Speed_Init();
    while(1){
        if(Protocol_Status.dst.Vr > 0){
            Protocol_Status.cur.Vr = R_CAR_GET_V(GetRightCount());
        }
        else if(Protocol_Status.dst.Vr < 0){
            Protocol_Status.cur.Vr = -(R_CAR_GET_V(GetRightCount()));
        }
        else{
            Protocol_Status.cur.Vr = 0;
        }
        
        if(Protocol_Status.dst.Vl > 0){
            Protocol_Status.cur.Vl = R_CAR_GET_V(GetLeftCount());
        }
        else if(Protocol_Status.dst.Vl < 0){
            Protocol_Status.cur.Vl = -(R_CAR_GET_V(GetLeftCount()));
        }
        else{
            Protocol_Status.cur.Vl = 0;
        }
        
        
        Protocol_Status.path.v = R_CAR_V(Protocol_Status.cur.Vr, Protocol_Status.cur.Vl) ;
        Protocol_Status.path.w = R_CAR_W(Protocol_Status.cur.Vr, Protocol_Status.cur.Vl);
        Protocol_Status.path.deg_w = R_CAR_DEG_W(Protocol_Status.path.w);        
        Protocol_Status.path.deg = R_CAR_DEG(Protocol_Status.path.deg_w);
        
        PRT("R:\t %4d   L:\t %4d\r\n", Protocol_Status.cur.Vr, Protocol_Status.cur.Vl);
        vTaskDelay(30/portTICK_RATE_MS); 
    }
}