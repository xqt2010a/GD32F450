#include "los.h"
#include "task_app.h"

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    Os_Init();
    
    CreatTask(Task_Init1, Task1, 1000);
    CreatTask(NULL, Task2, 2000);
        
    while(1)
    {
        Task_Scheduler();
    }
}
