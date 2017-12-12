#include "SEGGER_RTT.h"
#include "string.h"

#define BUF_LEN  64
#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)
uint8_t  Rx_size, Key_index;
uint8_t  Rx_buf[BUF_LEN];

uint8_t Key_Get_key(void)
{
    return Rx_buf[0];
}

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    memset(Rx_buf, 0, BUF_LEN);
    Key_index = 0;
    while(1)
    {
        if(SEGGER_RTT_HasKey()){
            Delay();
            Rx_size = SEGGER_RTT_HasData(0);
            SEGGER_RTT_Read(0, Rx_buf, Rx_size);
            Key_index = Key_Get_key();
            memset(Rx_buf, 0, Rx_size);
        }
        if(Key_index){
            pPage->Function(Key_index);
        }
    }
}
