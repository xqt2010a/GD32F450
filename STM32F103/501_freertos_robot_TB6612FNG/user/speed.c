#include "speed.h"
#include "string.h"


//�������ֲ�׽ͨ���벶��ֵ  TIM2ͨ��3 
#define  LEFTCAPTURECHANNEL TIM2, TIM_IT_CC3  
#define  LEFTCAPTUREVALUE TIM_GetCapture3(TIM2)  
//<span style="font-family: Arial, Helvetica, sans-serif;">TIM1ͨ��4</span>  
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
      //��������  
    GPIO_InitTypeDef WheelGPIO_InitStructure;  
    NVIC_InitTypeDef WheelNVIC_InitStructure;  
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_ICInitTypeDef  TIM_ICInitStructure;  
      
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); //ѡ��Timer2ȫ����ӳ��    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
      
      //�ٶȲ���ͨ��ʱ����ͨ������ ʱ�� 10000  
    TIM_TimeBaseStructure.TIM_Period = 60000;   //65535
    TIM_TimeBaseStructure.TIM_Prescaler = 720-1;  //����=72000000/(psc+1)��mix=0.01ms
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    
      /**************��������******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  //������
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
      
      //�����ٶȲ�������ܽ����� PB10
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM2, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);   
    /**************�������ý���******************/  
  
    /**************��������******************/  
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;  
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;  
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;  
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  
    TIM_ICInitStructure.TIM_ICFilter = 0x0f;  
        
        //�����ٶȲ�������ܽ����� PB11  
    WheelGPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;  
    WheelGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    WheelGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_Init(GPIOB, &WheelGPIO_InitStructure);  
      
    TIM_ICInit(TIM2, &TIM_ICInitStructure);  
    TIM_ITConfig(TIM2, TIM_IT_CC4, ENABLE);   
    /**************�������ý���******************/  
  
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
            RightPeriodBuf[RightPeriodIndex++] = RightWheelPulsePeriod;//��¼�����10��ֵ  
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
            LeftPeriodBuf[LeftPeriodIndex++] = LeftWheelPulsePeriod;//��¼�����10��ֵ  
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
  * @������ TIM1_IRQHandler 
  * @����   TIM1�жϴ����������Ӳ��ٲ���ģʽ 
  * @����   �� 
  * @����ֵ �� 
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