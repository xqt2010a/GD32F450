#include "SEGGER_RTT.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"

//#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

uint8_t Buf[20] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};

void delay_ms(u16 time)       
{      
    u16 i=0;      
    while(time--)       
    {      
        i=12000;      
        while(i--);      
    }      
}   

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    Uart_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�����жϷ��飬��Ϊ2�飺2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    while(1)
    {
        //printf("\n���͵���Ϣ��");  
        Uart_PutStr(USART1, "hello world!"); 
        Uart_PutStr(USART1, Buf); 
        delay_ms(1000);   
    }
}
