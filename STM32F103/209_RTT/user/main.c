#include "SEGGER_RTT.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"

#define BUF_LEN  64
#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)
uint8_t  Rx_size;
uint8_t  Rx_buf[BUF_LEN];

void Delay(void)
{
    uint32_t i;
    for(i=0; i<0xFFFF; i++);
}


void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    printf("RTT demo!\r\n");
    while(1)
    {
        if(SEGGER_RTT_HasKey()){
            Delay();
            Rx_size = SEGGER_RTT_HasData(0);
            SEGGER_RTT_Read(0, Rx_buf, Rx_size);
            SEGGER_RTT_WriteString(0,(char const *)Rx_buf);
            memset(Rx_buf, 0, Rx_size);
        }
    }
}
