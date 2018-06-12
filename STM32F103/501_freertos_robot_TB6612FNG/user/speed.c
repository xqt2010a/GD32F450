#include "speed.h"
#include "string.h"


//定义左轮捕捉通道与捕获值  TIM2通道3 
#define  LEFTCAPTURECHANNEL TIM2, TIM_IT_CC3  
#define  LEFTCAPTUREVALUE TIM_GetCapture3(TIM2)  

#define  RIGHTCAPTURECHANNEL TIM2, TIM_IT_CC4  
#define  RIGHTCAPTUREVALUE TIM_GetCapture4(TIM2)  

#define PERIOD_BUFSIZE      20
#define TIM_PERIOD_VALUE    60000
#define STOP_LEN            250000//178194//450
#define MIN_SPEED           80000
#define COUNT_RATE          5

uint16_t LeftPeriodIndex = 0;
uint16_t RightPeriodIndex = 0;

uint16_t RightPeriodBuf[PERIOD_BUFSIZE];
uint16_t LeftPeriodBuf[PERIOD_BUFSIZE];

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
    TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD_VALUE;   //65535
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

void ShutDown(char flag)
{
    if(1 == flag){
        if((ABS_FUC(Protocol_Status.dst_temp.Vr) - ABS_FUC(Protocol_Status.dst_temp.Vl))<5){
            if(Protocol_Status.dst_temp.Vr >=0){
                if(Protocol_Status.dst_temp.Vl >= 0){
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr-((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst.Vr;
                    if(Protocol_Status.dst.Vr < MIN_SPEED){
                        Protocol_Status.dst.Vr = MIN_SPEED;
                        Protocol_Status.dst.Vl = MIN_SPEED;
                    }
                }
                else{   //Protocol_Status.dst_temp.Vl < 0
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr-((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = -(Protocol_Status.dst.Vr);
                    if(Protocol_Status.dst.Vr < MIN_SPEED){
                        Protocol_Status.dst.Vr = MIN_SPEED;
                        Protocol_Status.dst.Vl = -MIN_SPEED;
                    }
                }
            }
            else{   //Protocol_Status.dst_temp.Vr < 0
                if(Protocol_Status.dst_temp.Vl >= 0){
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr+((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = -(Protocol_Status.dst.Vr);
                    if(Protocol_Status.dst.Vl < MIN_SPEED){
                        Protocol_Status.dst.Vr = -MIN_SPEED;
                        Protocol_Status.dst.Vl = MIN_SPEED;
                    }
                }
                else{   //Protocol_Status.dst_temp.Vl < 0
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr+((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst.Vr;
                    if(Protocol_Status.dst.Vl > -MIN_SPEED){
                        Protocol_Status.dst.Vr = -MIN_SPEED;
                        Protocol_Status.dst.Vl = -MIN_SPEED;
                    }
                }
            }
            
        }
        else{
            if(Protocol_Status.dst_temp.Vr >= 0){
                if(Protocol_Status.dst_temp.Vl >= 0){
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr-((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst_temp.Vl-((Protocol_Status.run_sl+STOP_LEN-Protocol_Status.Sl)*COUNT_RATE);
                    if(Protocol_Status.dst.Vr < MIN_SPEED){
                        Protocol_Status.dst.Vr = MIN_SPEED;
                    }
                    if(Protocol_Status.dst.Vl < MIN_SPEED){
                        Protocol_Status.dst.Vl = MIN_SPEED;
                    }
                }
                else{   //Protocol_Status.dst.Vl < 0
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr-((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst_temp.Vl+((Protocol_Status.run_sl+STOP_LEN-Protocol_Status.Sl)*COUNT_RATE);
                    if(Protocol_Status.dst.Vr < MIN_SPEED){
                        Protocol_Status.dst.Vr = MIN_SPEED;
                    }
                    if(Protocol_Status.dst.Vl > -MIN_SPEED){
                        Protocol_Status.dst.Vl = -MIN_SPEED;
                    }
                }
            }
            else{       //Protocol_Status.dst.Vr < 0
                if(Protocol_Status.dst_temp.Vl >= 0){
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr+((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst_temp.Vl-((Protocol_Status.run_sl+STOP_LEN-Protocol_Status.Sl)*COUNT_RATE);
                    if(Protocol_Status.dst.Vr > -MIN_SPEED){
                        Protocol_Status.dst.Vr = -MIN_SPEED;
                    }
                    if(Protocol_Status.dst.Vl < MIN_SPEED){
                        Protocol_Status.dst.Vl = MIN_SPEED;
                    }
                }
                else{   //Protocol_Status.dst.Vl < 0
                    Protocol_Status.dst.Vr = Protocol_Status.dst_temp.Vr+((Protocol_Status.run_sr+STOP_LEN-Protocol_Status.Sr)*COUNT_RATE);
                    Protocol_Status.dst.Vl = Protocol_Status.dst_temp.Vl+((Protocol_Status.run_sl+STOP_LEN-Protocol_Status.Sl)*COUNT_RATE);
                    if(Protocol_Status.dst.Vr > -MIN_SPEED){
                        Protocol_Status.dst.Vr = -MIN_SPEED;
                    }
                    if(Protocol_Status.dst.Vl > -MIN_SPEED){
                        Protocol_Status.dst.Vl = -MIN_SPEED;
                    }
                }
            }
            
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
    char flag = 0;
    uint16_t capture_value, value;
    static uint16_t last_capture_r = 0, last_capture_l = 0;
    
    if(TIM_GetITStatus(RIGHTCAPTURECHANNEL) == SET){
        TIM_ClearITPendingBit(RIGHTCAPTURECHANNEL);
        capture_value = RIGHTCAPTUREVALUE;
        if(capture_value >= last_capture_r){
            value = capture_value - last_capture_r;
        }
        else{
            value = TIM_PERIOD_VALUE - last_capture_r + capture_value;
        }
        last_capture_r = capture_value;
        RightPeriodBuf[RightPeriodIndex++] = value;     //记录最近的 n 个值  
        if(RightPeriodIndex == PERIOD_BUFSIZE) 
            RightPeriodIndex = PERIOD_BUFSIZE-1;
        
        Right_Count++;  //用于行走距离
        if((R_VS_DOWN_CMD == Protocol_Status.cmd_type)||(R_WD_DOWN_CMD == Protocol_Status.cmd_type)){
            Protocol_Status.run_sr = (((uint64_t)(R_CAR_PI)*(R_CAR_LEN_R)*Right_Count+R_CAR_ENCODER_N*50)/((R_CAR_ENCODER_N)*100));
            if(Protocol_Status.run_sr > Protocol_Status.Sr){
                Protocol_Status.dst.Vr = 0;
                Protocol_Status.dst.Vl = 0;
            }
            else if(Protocol_Status.run_sr > (Protocol_Status.Sr-STOP_LEN)){
                flag = 1;
            }
        }
    }
    if(TIM_GetITStatus(LEFTCAPTURECHANNEL) == SET)   
    {   
        TIM_ClearITPendingBit(LEFTCAPTURECHANNEL);
        capture_value = LEFTCAPTUREVALUE;
        if(capture_value >= last_capture_l){
            value = capture_value - last_capture_l;
        }
        else{
            value = TIM_PERIOD_VALUE - last_capture_l + capture_value;
        }
        last_capture_l = capture_value;
        LeftPeriodBuf[LeftPeriodIndex++] = value;//记录最近的10个值
        if(LeftPeriodIndex == PERIOD_BUFSIZE)   
            LeftPeriodIndex = PERIOD_BUFSIZE-1;
        Left_Count++;   //用于行走距离
        if((R_VS_DOWN_CMD == Protocol_Status.cmd_type)||(R_WD_DOWN_CMD == Protocol_Status.cmd_type)){
            Protocol_Status.run_sl = (((uint64_t)(R_CAR_PI)*(R_CAR_LEN_L)*Left_Count+R_CAR_ENCODER_N*50)/((R_CAR_ENCODER_N)*100));
            if(Protocol_Status.run_sl > Protocol_Status.Sl){
                Protocol_Status.dst.Vl = 0;
                Protocol_Status.dst.Vr = 0;
            }
            else if(Protocol_Status.run_sl > (Protocol_Status.Sl-STOP_LEN)){
                flag = 1;
            }
        }
    }
    ShutDown(flag); 
}  


unsigned int GetLeftCount(void)  
{  
    uint16_t i;
    uint32_t sum=0, LeftValueAvg = 0;
    for(i=0; i<LeftPeriodIndex; i++){
        sum += LeftPeriodBuf[i];
        //LeftPeriodBuf[i] = 0;
    }
    LeftValueAvg = sum/LeftPeriodIndex;
    for(i=0; i<LeftPeriodIndex; i++){
        LeftPeriodBuf[i] = 0;
    }
    LeftPeriodIndex = 0;
    return LeftValueAvg;  
}  
  
unsigned int GetRightCount(void)  
{  
    uint16_t i;
    uint32_t sum = 0, RightValueAvg = 0;
    for(i=0; i<RightPeriodIndex; i++){
        sum += RightPeriodBuf[i];
        //RightPeriodBuf[i] = 0;
    }
    RightValueAvg = sum/RightPeriodIndex;  
    for(i=0; i<RightPeriodIndex; i++){
        RightPeriodBuf[i] = 0;
    }
    RightPeriodIndex = 0;
    return RightValueAvg;  
}  


void Speed_Init(void)
{
    Capture_Init();
    memset(RightPeriodBuf, 0, PERIOD_BUFSIZE);
    memset(LeftPeriodBuf, 0, PERIOD_BUFSIZE);
}

void vTask_Speed(void *p)
{
    Speed_Init();
    Protocol_Init();

    while(1){
        if(Protocol_Status.dst.Vr >= 0){            
            Protocol_Status.cur.Vr = R_CAR_GET_Vr(GetRightCount());
        }
        else{
            Protocol_Status.cur.Vr = (-(R_CAR_GET_Vr(GetRightCount())));
        }
        
        if(Protocol_Status.dst.Vl >= 0){
            Protocol_Status.cur.Vl = R_CAR_GET_Vl(GetLeftCount());
        }
        else{
            Protocol_Status.cur.Vl = (-(R_CAR_GET_Vl(GetLeftCount())));
        }
        
        
        Protocol_Status.path.v = R_CAR_V(Protocol_Status.cur.Vr, Protocol_Status.cur.Vl) ;
        Protocol_Status.path.w = R_CAR_W(Protocol_Status.cur.Vr, Protocol_Status.cur.Vl);
        Protocol_Status.path.deg_w = R_CAR_DEG_W(Protocol_Status.path.w);        
        Protocol_Status.path.deg = R_CAR_DEG(Protocol_Status.path.deg_w);
        
        //PRT("R:\t %4d   L:\t %4d\r\n", Protocol_Status.cur.Vr, Protocol_Status.cur.Vl);
        vTaskDelay(20/portTICK_RATE_MS); 
    }
}