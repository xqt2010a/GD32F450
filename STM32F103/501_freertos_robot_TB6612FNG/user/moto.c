#include "FreeRTOS.h"
#include "task.h"
#include "moto.h"
#include "robot_protocol.h"

void GPIOB_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //ѡ��Timer3������ӳ��    
    //ѡ��ʱ��3��ͨ��2��ΪPWM���������TIM3_CH2->PB5    GPIOB.5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_5; //PB0,PB5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�������칦��
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ������
}

void TIM3_Set_Frq(uint16_t arr, uint16_t psc)  /* Hz = 72000000/(arr+1)/(psc+1) */
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//�����ʼ���ṹ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ�ܶ�ʱ��3ʱ��
    //��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; //�Զ���װ�ؼĴ�����ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //TIMXԤ��Ƶ��ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //�������Ϲ��ܶԶ�ʱ�����г�ʼ��
}

void TIM3_Channel_Init(void)
{
    TIM_OCInitTypeDef  TIM_OCInitStructure;//����ṹ��
    
    TIM_OCStructInit(&TIM_OCInitStructure);//����ȱʡֵ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//����Ƚϼ��Ե�
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);//���ݽṹ����Ϣ���г�ʼ��CH1
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);//���ݽṹ����Ϣ���г�ʼ��CH2
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ�ܶ�ʱ��TIM3��CCR1�ϵ�Ԥװ��ֵ
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ�ܶ�ʱ��TIM3��CCR2�ϵ�Ԥװ��ֵ
    TIM_Cmd(TIM3, ENABLE);  //ʹ�ܶ�ʱ��TIM3
}


void Moto_Init(void)
{
    GPIOB_Init();
    TIM3_Set_Frq(9999,71);    //Ƶ��Ϊ��72*10^6/(9999+1)/(71+1)=100Hz
    TIM3_Channel_Init();
    TIM_SetCompare3(TIM3,1); //CH1�õ�ռ�ձ�Ϊ50%��pwm���� PB0
    TIM_SetCompare2(TIM3,1); //CH2�õ�ռ�ձ�Ϊ50%��pwm���� PB5
}

void vTask_Moto(void *p)
{
    //uint16_t i=0;
    //char flag = 0;
    uint32_t v = 0;
    
    while(1){
        //TIM_SetCompare2(TIM3,i); //�õ�ռ�ձ�Ϊ50%��pwm����
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
