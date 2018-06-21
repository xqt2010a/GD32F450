#include "SEGGER_RTT.h"
#include "string.h"
#include "si7021.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

int32_t SI7021_T, SI7021_RH;

void main(void)
{
    uint32_t i;
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    SI7021_Init();

    while(1)
    {
        SI7021_T = Read_T();
        SI7021_RH = Read_RH();
        PRT("Temperature:%d.%d¡æ  Humidity:%d.%d%%\r\n",SI7021_T/100, 
            (SI7021_T>=0?SI7021_T:(-SI7021_T))%100, SI7021_RH/100, 
            (SI7021_RH>=0?SI7021_RH:(-SI7021_RH))%100);
        
        for(i=0; i<100; i++)
            delay_us(10000);
    }
}
