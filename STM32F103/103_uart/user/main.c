#include "SEGGER_RTT.h"
#include "string.h"
#include "uart.h"

//#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    Uart_Init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //进行中断分组，分为2组：2位抢占优先级，2位响应优先级
    while(1)
    {
    
    }
}
