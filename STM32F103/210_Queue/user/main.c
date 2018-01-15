#include "SEGGER_RTT.h"
#include "string.h"
#include "queue.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)
#define Queue_Len   20

uint8_t Buf[20]="Labc New Date!\r\n";
uint8_t Buf_T;

void main(void)
{
    uint8_t i;
    Queue Q;
    
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    GreateQueue(&Q, Queue_Len);
    while(1)
    {
        for(i=0; i<Queue_Len; i++){
            EnQueue(&Q, Buf[i]);    
        }
        for(i=0; i<Queue_Len; i++){
            DeQueue(&Q,&Buf_T);
            PRT("%c",Buf_T);
        }
        while(1);
    }
}
