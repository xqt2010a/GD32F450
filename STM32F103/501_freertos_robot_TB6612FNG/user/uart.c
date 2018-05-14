#include "uart.h"

void Uart_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStrue;
    USART_InitTypeDef USART_InitStrue;
    NVIC_InitTypeDef NVIC_InitStrue;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //此处与 UART1 不同
    
    
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;       //复用功能推挽输出模式
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_2;             //PA2  USART2_Tx
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;     //设置输出速度
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING; //浮空输入模式
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_3;            //PA3  USART2_Rx
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    
    USART_DeInit(USART2);                           //USART1串口复位
    USART_InitStrue.USART_BaudRate=115200;          //设置波特率9600
    USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//无硬件数据流控制，相关寄存器为CR3
    USART_InitStrue.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; //模式为接收和发送
    USART_InitStrue.USART_Parity=USART_Parity_No;           //无奇偶校验位0x0000，相关寄存器为SR1的PS位
    USART_InitStrue.USART_StopBits=USART_StopBits_1;        //1个停止位
    USART_InitStrue.USART_WordLength=USART_WordLength_8b;   //数据长度为8位
    USART_Init(USART2,&USART_InitStrue);                    //串口初始化
    
    
    NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;             //串口2对应的中断名称
    NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;               //使能该中断通道
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=2;     //抢占优先级为2，因为此程序只有一个中断，故可任意设置
    NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;            //响应优先级为1，因为此程序只有一个中断，故可任意设置
    NVIC_Init(&NVIC_InitStrue);                             //中断初始化
    
    USART_Cmd(USART2,ENABLE);                               //串口使能  就是将寄存器CR1中的UE位置1
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);            //使能串口中断，也叫开启串口中断，第二个参数是设置具体开启哪种中断，具体为寄存器SR
}

void Uart_StrSend(uint8_t *pbuf, uint8_t len)
{
    uint8_t i;
    for(i=0; i<len; i++){
        //while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
        USART_SendData(USART2, (uint8_t)pbuf[i]);
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
    }
}