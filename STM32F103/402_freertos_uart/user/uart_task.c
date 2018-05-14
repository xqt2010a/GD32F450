#include "uart_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "robot_protocol.h"


uint8_t Uart1_Buf_Rx[UART_BUF_LEN];
uint8_t Uart1_Sta_Rx = 0;
uint8_t Uart1_Index_Rx = 0;

uint8_t Uart2_Buf_Rx[UART_BUF_LEN];
uint8_t Uart2_Sta_Rx = 0;
uint8_t Uart2_Index_Rx = 0;


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE)) //判断如果收到了数据，收到数据CR1中RXNE位为1，该函数的返回值为1
    {
        Uart1_Buf_Rx[Uart1_Index_Rx++] = USART_ReceiveData(USART1);         //读取数据
        Uart1_Sta_Rx = 1;
    }
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2,USART_IT_RXNE)) //判断如果收到了数据，收到数据CR1中RXNE位为1，该函数的返回值为1
    {
        Uart2_Buf_Rx[Uart2_Index_Rx++] = USART_ReceiveData(USART2);         //读取数据
        Uart2_Sta_Rx = 1;
    }
}

void vUart1_Task(void *p)
{
    Uart1_Init();
    
    while(1){
        if(Uart1_Sta_Rx){
            vTaskDelay(10/portTICK_RATE_MS);    //10ms ,115200, 14.4B/s
            USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);    //关闭串口接收中断
            
            EncodeProcess(Uart1_Buf_Rx);
            
            Uart1_Sta_Rx = 0;
            Uart1_Index_Rx = 0;
            USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);    //打开串口接收中断
        }
    }
}

void vUart1_Report_Task(void *p)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();        //获取当前tick
    while(1){
        
        vTaskDelayUntil( &xLastWakeTime, 5000/portTICK_RATE_MS );
        PathReport();
    }
}

void vUart2_Task(void *p)
{
    Uart2_Init();
    
    while(1){
        if(Uart2_Sta_Rx){
            vTaskDelay(10/portTICK_RATE_MS);    //10ms ,115200, 14.4B/s
            USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);    //关闭串口接收中断
            
            //EncodeProcess(Uart2_Buf_Rx);
            Uart2_StrSend("Uart2_Buf_Rx\r\n", 14);
            Uart2_Sta_Rx = 0;
            Uart2_Index_Rx = 0;
            USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);    //打开串口接收中断
        }
    }
}

void vUart2_Report_Task(void *p)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();        //获取当前tick
    while(1){
        
        vTaskDelayUntil( &xLastWakeTime, 1000/portTICK_RATE_MS );
        Uart2_StrSend("hello_labc!\r\n", 13);
    }
}
