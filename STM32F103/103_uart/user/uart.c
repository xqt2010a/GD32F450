#include "uart.h"

void Uart_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStrue;
    USART_InitTypeDef USART_InitStrue;
    NVIC_InitTypeDef NVIC_InitStrue;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
    
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;       //���ù����������ģʽ
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_9;             //PA9  USART1_Tx
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;     //��������ٶ�
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING; //��������ģʽ
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;            //PA10  USART1_Rx
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    
    USART_DeInit(USART1);                           //USART1���ڸ�λ
    USART_InitStrue.USART_BaudRate=115200;          //���ò�����9600
    USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ�����������ƣ���ؼĴ���ΪCR3
    USART_InitStrue.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; //ģʽΪ���պͷ���
    USART_InitStrue.USART_Parity=USART_Parity_No;           //����żУ��λ0x0000����ؼĴ���ΪSR1��PSλ
    USART_InitStrue.USART_StopBits=USART_StopBits_1;        //1��ֹͣλ
    USART_InitStrue.USART_WordLength=USART_WordLength_8b;   //���ݳ���Ϊ8λ
    USART_Init(USART1,&USART_InitStrue);                    //���ڳ�ʼ��
    
    
    NVIC_InitStrue.NVIC_IRQChannel=USART1_IRQn;             //����1��Ӧ���ж�����
    NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;               //ʹ�ܸ��ж�ͨ��
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;     //��ռ���ȼ�Ϊ2����Ϊ�˳���ֻ��һ���жϣ��ʿ���������
    NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;            //��Ӧ���ȼ�Ϊ1����Ϊ�˳���ֻ��һ���жϣ��ʿ���������
    NVIC_Init(&NVIC_InitStrue);                             //�жϳ�ʼ��
    
    USART_Cmd(USART1,ENABLE);                               //����ʹ��  ���ǽ��Ĵ���CR1�е�UEλ��1
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);            //ʹ�ܴ����жϣ�Ҳ�п��������жϣ��ڶ������������þ��忪�������жϣ�����Ϊ�Ĵ���SR
}

void USART1_IRQHandler(void)
{
    u16 DATA;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)) //�ж�����յ������ݣ��յ�����CR1��RXNEλΪ1���ú����ķ���ֵΪ1
    {
        DATA=USART_ReceiveData(USART1);         //��ȡ����
        USART_SendData(USART1,DATA);            //���Ͷ���������
    }
}