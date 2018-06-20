#include "SEGGER_RTT.h"
#include "string.h"
#include "si7021.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

uint32_t SI7021_T, SI7021_RH;

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    SI7021_Init();
    
    SI7021_T = Read_T();
    SI7021_RH = Read_RH();
    while(1)
    {
    
    }
}
