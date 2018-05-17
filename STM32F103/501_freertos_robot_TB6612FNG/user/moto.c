#include "FreeRTOS.h"
#include "task.h"
#include "moto.h"
#include "robot_protocol.h"
#include "pid.h"

#define SPEED_COUNT_RATE    30      //�ٶ� �� �������ֵ
#define ARR_VALUE           10000   //�Զ���װ�ؼĴ�����ֵ

void GPIO_Init_Moto(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1, ENABLE); //ѡ��Timer1������ӳ��    
    //ѡ��ʱ��1��ͨ��1,2��ΪPWM���������TIM1_CH1->PA8    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //PA8,PA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�������칦��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ������
    
    /* PC0 PC1 ���� 1 - ����   PC2 PC3 ���� 1 - �ҵ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��GPIO����ģ��ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ; //PC0��1��2��3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //���칦��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ������
    
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);   //PC0��1��2��3 = 0;

}

void TIM1_Channel_Init(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;//����ṹ��
    
    TIM_OCStructInit(&TIM_OCInitStructure);//����ȱʡֵ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//����Ƚϼ��Ե�
    
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);//���ݽṹ����Ϣ���г�ʼ��CH1
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);//���ݽṹ����Ϣ���г�ʼ��CH2
    
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ�ܶ�ʱ��TIM1��CCR1�ϵ�Ԥװ��ֵ
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ�ܶ�ʱ��TIM1��CCR2�ϵ�Ԥװ��ֵ
    
    TIM_Cmd(TIM1, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}

void TIM1_Set_Frq(uint16_t arr, uint16_t psc)  /* Hz = 72000000/(arr+1)/(psc+1) */
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//�����ʼ���ṹ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʹ�ܶ�ʱ��3ʱ��
    //��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //�Զ���װ�ؼĴ�����ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //TIMXԤ��Ƶ��ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //�������Ϲ��ܶԶ�ʱ�����г�ʼ��
}

void Moto1_Left_Stop(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1);  //PC0, PC1 = 0;
}

void Moto1_Right_Stop(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_3);  //PC2, PC3 = 0;
}

void Moto1_Left_Forword(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_0);    //PC0 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);  //PC1 = 0;
}

void Moto1_Right_Forword(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_2);    //PC2 = 1;
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);  //PC3 = 0;
}

void Moto1_Left_Back(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_0);    //PC0 = 0;
    GPIO_ResetBits(GPIOC, GPIO_Pin_1);  //PC1 = 1;
}

void Moto1_Right_Back(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_2);    //PC2 = 0;
    GPIO_ResetBits(GPIOC, GPIO_Pin_3);  //PC3 = 1;
}

void Moto1_Left_Speed(uint16_t Vs)
{
    TIM_SetCompare2(TIM1, Vs);  
    Moto1_Left_Forword();
}

void Moto1_Right_Speed(uint16_t Vs)
{
    TIM_SetCompare1(TIM1, Vs);
    Moto1_Right_Forword();
}

void Moto_Init(void)
{
    GPIO_Init_Moto();
    TIM1_Set_Frq(ARR_VALUE-1,71);    //Ƶ��Ϊ��72*10^6/(9999+1)/(71+1)=100Hz
    TIM1_Channel_Init();
    TIM_SetCompare1(TIM1,ARR_VALUE/2-1); //CH1�õ�ռ�ձ�Ϊ50%��pwm���� PB0
    TIM_SetCompare2(TIM1,ARR_VALUE/2-1); //CH2�õ�ռ�ձ�Ϊ50%��pwm���� PB5
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    //Moto1_Left_Stop();
    //Moto1_Right_Stop();
    Moto1_Left_Forword();
    Moto1_Right_Forword();
    
    //Moto1_Left_Back();
    //Moto1_Right_Back();
    
    
}



void vTask_Moto(void *p)
{
    int32_t v = 0;
    Moto_Init();
    while(1){
        if(0 != Protocol_Status.dst.Vl){    //Left PWM
            v = PID_realize_L(Protocol_Status.dst.Vl, Protocol_Status.cur.Vl);
            if(v >= (SPEED_COUNT_RATE*ARR_VALUE)){
                v = (SPEED_COUNT_RATE*ARR_VALUE);
            }
            else if(v < SPEED_COUNT_RATE){
                v = SPEED_COUNT_RATE;
            }
            Moto1_Left_Speed(v/SPEED_COUNT_RATE-1);
            
        }
        else{
            Moto1_Left_Stop();
            TIM_SetCompare2(TIM1, 0);
        }
        
        if(0 != Protocol_Status.dst.Vr){    //Right PWM
            v = PID_realize_R(Protocol_Status.dst.Vr, Protocol_Status.cur.Vr);
            if(v >= (SPEED_COUNT_RATE*ARR_VALUE)){
                v = (SPEED_COUNT_RATE*ARR_VALUE)-1;
            }
            else if(v < SPEED_COUNT_RATE){
                v = SPEED_COUNT_RATE;
            }
            Moto1_Right_Speed(v/SPEED_COUNT_RATE-1);
        }
        else{
            Moto1_Right_Stop();
            TIM_SetCompare1(TIM1,0);
        }
        vTaskDelay(50/portTICK_RATE_MS);
    }
}
