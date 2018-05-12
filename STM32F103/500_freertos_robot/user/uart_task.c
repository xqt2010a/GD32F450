#include "uart_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "robot_protocol.h"


uint8_t Uart_Buf_Rx[UART_BUF_LEN];
uint8_t Uart_Sta_Rx = 0;
uint8_t Uart_Index_Rx = 0;


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE)) //判断如果收到了数据，收到数据CR1中RXNE位为1，该函数的返回值为1
    {
        Uart_Buf_Rx[Uart_Index_Rx++] = USART_ReceiveData(USART1);         //读取数据
        Uart_Sta_Rx = 1;
    }
}

void vUart_Task(void *p)
{
    Uart_Init();
    
    while(1){
        if(Uart_Sta_Rx){
            vTaskDelay(10/portTICK_RATE_MS);    //10ms ,115200, 14.4B/s
            USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);    //关闭串口接收中断
            
            EncodeProcess(Uart_Buf_Rx);
            
            Uart_Sta_Rx = 0;
            Uart_Index_Rx = 0;
            USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);    //打开串口接收中断
        }
    }
}

void vReport_Task(void *p)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();        //获取当前tick
    while(1){
        
        vTaskDelayUntil( &xLastWakeTime, 5000/portTICK_RATE_MS );
        PathReport();
    }
}