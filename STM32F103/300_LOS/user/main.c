#include "SEGGER_RTT.h"
#include "string.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    while(1)
    {
    
    }
}
