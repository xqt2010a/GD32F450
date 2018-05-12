#include "SEGGER_RTT.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "moto.h"
#include "speed.h"
#include "uart_task.h"

//#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

void vTask(void *pvParameters)  
{  
    int i=0;
     while(1)  
     {  
         PRT("hello world!%6d\r\n",i++);
         vTaskDelay(1000/portTICK_RATE_MS);  
     }  
}


void main(void)
{
    Moto_Init();
    Speed_Init();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    xTaskCreate(vTask,"Task1",50,NULL,1,NULL); 
    xTaskCreate(vTask_Moto,"Task_Moto",50,NULL,1,NULL); 
    xTaskCreate(vTask_Speed,"Task_Speed",50,NULL,1,NULL); 
    xTaskCreate(vUart_Task,"Uart_Task",100,NULL,1,NULL); 
    xTaskCreate(vReport_Task,"Report_Task",50,NULL,1,NULL);
    vTaskStartScheduler(); 
    while(1){
    
    }
}
