#include "FreeRTOS.h"
#include "task.h"
#include "moto.h"
#include "robot_protocol.h"
#include "pid.h"
#include "speed.h"
#include "string.h"

#define SPEED_COUNT_RATE    30      //速度 和 捕获计数值
#define ARR_VALUE           9999   //自动重装载寄存器的值

static int32_t Vl_last = 0, Vr_last = 0; 
int32_t Vl, Vr, abs_Vl, abs_Vr, abs_dst_Vl, abs_dst_Vr, abs_cur_Vl, abs_cur_Vr;
uint8_t com_flag=0;

void GPIO_Init_Moto(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//使能GPIO外设和AFIO复用功能模块时钟
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE); //选择Timer1部分重映像    
    //选择定时器1的通道1,2作为PWM的输出引脚TIM1_CH1->PA8    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //PA8,PA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化引脚
    
    /* PC0 PC1 控制 1 - 左电机   PC2 PC3 控制 1 - 右电机 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能GPIO外设模块时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ; //PC0，1，2，3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化引脚
    
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);   //PC0，1，2，3 = 0;

}

void TIM1_Channel_Init(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;//定义结构体
    
    TIM_OCStructInit(&TIM_OCInitStructure);//设置缺省值
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//选择定时器模式，TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出比较极性低
    
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//根据结构体信息进行初始化CH1
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);//根据结构体信息进行初始化CH2
    
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能定时器TIM1在CCR1上的预装载值
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能定时器TIM1在CCR2上的预装载值
    
    TIM_Cmd(TIM1, ENABLE);  //使能定时器TIM3
}

void TIM1_Set_Frq(uint16_t arr, uint16_t psc)  /* Hz = 72000000/(arr+1)/(psc+1) */
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定义初始化结构体
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //使能定时器3时钟
    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //自动重装载寄存器的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //TIMX预分频的值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据以上功能对定时器进行初始化
}

void Moto1_Left_Stop(void)
{
    TIM_SetCompare2(TIM1, 0);          //Left
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1);  //PC0, PC1 = 0;
}

void Moto1_Right_Stop(void)
{
    TIM_SetCompare1(TIM1, 0);          //Right
    GPIO_ResetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3);  //PC2, PC3 = 0;
}

void Moto1_Left_Back(uint32_t Vs)
{
    TIM_SetCompare2(TIM1, Vs/SPEED_COUNT_RATE);          //Left
    GPIO_SetBits(GPIOC, GPIO_Pin_1);    //PC1 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_0);  //PC0 = 0;
}

void Moto1_Right_Back(uint32_t Vs)
{
    TIM_SetCompare1(TIM1, Vs/SPEED_COUNT_RATE);          //Right
    GPIO_SetBits(GPIOC, GPIO_Pin_3);    //PC3 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_2);  //PC2 = 0;
}

void Moto1_Left_Forword(uint32_t Vs)
{
    TIM_SetCompare2(TIM1, Vs/SPEED_COUNT_RATE);          //Left
    GPIO_SetBits(GPIOC, GPIO_Pin_0);    //PC0 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);  //PC1 = 0;
}

void Moto1_Right_Forword(uint32_t Vs)
{
    TIM_SetCompare1(TIM1, Vs/SPEED_COUNT_RATE);          //Right
    GPIO_SetBits(GPIOC, GPIO_Pin_2);    //PC2 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);  //PC3 = 0;
}


void Moto_Init(void)
{
    GPIO_Init_Moto();
    TIM1_Set_Frq(ARR_VALUE,71);    //频率为：72*10^6/(9999+1)/(71+1)=100Hz
    TIM1_Channel_Init();
    TIM_SetCompare1(TIM1,ARR_VALUE/2); //CH1得到占空比为50%的pwm波形 PB0
    TIM_SetCompare2(TIM1,ARR_VALUE/2); //CH2得到占空比为50%的pwm波形 PB5
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    //Moto1_Left_Stop();
    //Moto1_Right_Stop();
    //Moto1_Left_Forword();
    //Moto1_Right_Forword();
    
    //Moto1_Left_Back();
    //Moto1_Right_Back();
    
    
}


int32_t Correct_Count(uint32_t sr, uint32_t sl)    //矫正左右轮运行误差
{
    if((ABS_FUC(Protocol_Status.dst.Vl) - ABS_FUC(Protocol_Status.dst.Vr))<5){
        return PID_Correct_Len(sr, sl);
    }
    return 0;
}

void vTask_Moto(void *p)
{  
    static uint8_t r_i=0, l_i=0;
    uint32_t run_sl, run_sr;
    
    Moto_Init();
    
    while(1){
        //l_count = Left_Count;
        //r_count = Right_Count;
        run_sl = Protocol_Status.run_sl;
        run_sr = Protocol_Status.run_sr;
        if(0 != Protocol_Status.dst.Vl){    //left run    
            com_flag = 0;
            Vl = 0;
            abs_dst_Vl = ABS_FUC(Protocol_Status.dst.Vl);
            abs_cur_Vl = ABS_FUC(Protocol_Status.cur.Vl);
            
            if(0 != abs_dst_Vl){
                Vl = Vl_last + PID_realize_L(abs_dst_Vl, abs_cur_Vl, run_sl)/1000;
            }
            Vl += Correct_Count(run_sr, run_sl);
            abs_Vl = ABS_FUC(Vl);
            if(abs_Vl > SPEED_COUNT_RATE*ARR_VALUE){
                abs_Vl = SPEED_COUNT_RATE*ARR_VALUE;
            }
            if(Protocol_Status.dst.Vl >= 0){
                if(Vl < 0){
                    abs_Vl = 0;
                }         
                Vl_last = abs_Vl;
                Moto1_Left_Forword(abs_Vl);
            }
            else{
                if(Vl < 0){
                    abs_Vl = 0;
                }
                Vl_last = abs_Vl;
                Moto1_Left_Back(abs_Vl);
            }
        }
        else{
            if(1 == com_flag){
                vTaskDelay(50/portTICK_RATE_MS);
                pid_buf[4] = 0x02;      //左
                memcpy(pid_buf+33,(uint8_t *)&run_sl, 4);
                Uart_StrSend(pid_buf, PID_BUF_LEN);
                l_i++;
                if(l_i>20){
                    com_flag = 2;
                    l_i = r_i = 0;
                }
            }
        }
                                                  
        if(0 != Protocol_Status.dst.Vr){        //right run
            com_flag = 0;
            Vr = 0;      
            abs_dst_Vr = ABS_FUC(Protocol_Status.dst.Vr);
            abs_cur_Vr = ABS_FUC(Protocol_Status.cur.Vr);
            if(0 != abs_dst_Vr){
                Vr = Vr_last + PID_realize_R(abs_dst_Vr, abs_cur_Vr, run_sr)/1000;
            }
            abs_Vr = ABS_FUC(Vr);
            if(abs_Vr > SPEED_COUNT_RATE*ARR_VALUE){
                abs_Vr = SPEED_COUNT_RATE*ARR_VALUE;
            }
            if(Protocol_Status.dst.Vr >= 0){
                if(Vr < 0){
                    abs_Vr = 0;
                }
                Vr_last = abs_Vr;
                Moto1_Right_Forword(abs_Vr);
            }
            else{
                if(Vr < 0){
                    abs_Vr = 0;
                }
                Vr_last = abs_Vr;
                Moto1_Right_Back(abs_Vr);
            }
        }
        else{
            if(1 == com_flag){
                vTaskDelay(50/portTICK_RATE_MS);
                pid_buf[4] = 0x01;      //右
                memcpy(pid_buf+33,(uint8_t *)&run_sr, 4);
                Uart_StrSend(pid_buf, PID_BUF_LEN);
                r_i++;
                if(r_i>20){
                    com_flag = 2;
                    r_i = l_i = 0;
                }
            }
        }
        
        vTaskDelay(20/portTICK_RATE_MS);
    }
}

void vTask_Moto_Stop(void *p)
{
    uint8_t flag;
    
    while(1){
        flag = 0;       
        
        if(0 == Protocol_Status.dst.Vl){      //left stop
            if(0 != Protocol_Status.cur.Vl){
                Protocol_Status.dst.Vl = 0;
                //Protocol_Status.cur.Vl = 0;
                flag = 1;
                com_flag = 1;
            }
        }
    
        if(0 == Protocol_Status.dst.Vr){     //right stop
            if(0 != Protocol_Status.cur.Vr){
                Protocol_Status.dst.Vr = 0;
                //Protocol_Status.cur.Vr = 0;
                flag = 1;
                com_flag = 1;
            }
        }
        if(1 == flag){                //stop at the same time
            Moto1_Left_Stop();
            Moto1_Right_Stop();
            Protocol_Status.dst.Vr = 0;
            Protocol_Status.dst.Vl = 0;
            Protocol_Status.dst_temp.Vr = 0;
            Protocol_Status.dst_temp.Vr = 0;
        }
    }
}
