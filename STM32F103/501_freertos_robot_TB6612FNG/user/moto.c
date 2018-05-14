#include "FreeRTOS.h"
#include "task.h"
#include "moto.h"
#include "robot_protocol.h"

void GPIOB_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);//使能GPIO外设和AFIO复用功能模块时钟
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //选择Timer3部分重映像    
    //选择定时器3的通道2作为PWM的输出引脚TIM3_CH2->PB5    GPIOB.5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_5; //PB0,PB5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化引脚
}

void TIM3_Set_Frq(uint16_t arr, uint16_t psc)  /* Hz = 72000000/(arr+1)/(psc+1) */
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义初始化结构体
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟
    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //自动重装载寄存器的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //TIMX预分频的值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据以上功能对定时器进行初始化
}

void TIM3_Channel_Init(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;//定义结构体
    
    TIM_OCStructInit(&TIM_OCInitStructure);//设置缺省值
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//选择定时器模式，TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出比较极性低
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);//根据结构体信息进行初始化CH1
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);//根据结构体信息进行初始化CH2
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能定时器TIM3在CCR1上的预装载值
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能定时器TIM3在CCR2上的预装载值
    TIM_Cmd(TIM3, ENABLE);  //使能定时器TIM3
}


void Moto_Init(void)
{
    GPIOB_Init();
    TIM3_Set_Frq(9999,71);    //频率为：72*10^6/(9999+1)/(71+1)=100Hz
    TIM3_Channel_Init();
    TIM_SetCompare3(TIM3,1); //CH1得到占空比为50%的pwm波形 PB0
    TIM_SetCompare2(TIM3,1); //CH2得到占空比为50%的pwm波形 PB5
}

void vTask_Moto(void *p)
{
    //uint16_t i=0;
    //char flag = 0;
    uint32_t v = 0;
    
    while(1){
        //TIM_SetCompare2(TIM3,i); //得到占空比为50%的pwm波形
        //vTaskDelay(2);
        //i++;
        //if(i>9999)
        //    i = 0;
        if(R_CTRL_DOWN_CMD == Protocol_Status.cmd_type){
            v = ABS_FUC(Protocol_Status.ctrl.v);
            if(v > 9999){
                v = 9999;
            }
            TIM_SetCompare3(TIM3,v);
            TIM_SetCompare2(TIM3,v);
            Protocol_Status.cmd_type = 0;
        }
        else if(R_MODE_DOWN_CMD == Protocol_Status.cmd_type){
            TIM_SetCompare3(TIM3,1);
            TIM_SetCompare2(TIM3,1);
            Protocol_Status.cmd_type = 0;
        }
    }
}
