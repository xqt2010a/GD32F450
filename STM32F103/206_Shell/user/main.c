#include "SEGGER_RTT.h"
#include "Q_Shell.h"
#include "string.h"
#include "stdint.h"

#define BUF_LEN  64
#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)
uint8_t  Rx_size;
uint8_t  Rx_buf[BUF_LEN];

unsigned int Add(unsigned a,unsigned b)
{
    return (a+b);
}

void Delay(void)
{
    uint32_t i;
    for(i=0; i<0xFFFF; i++);
}

QSH_VAR_REG(Rx_size,"unsigned char  Rx_size","uint32_t");
QSH_FUN_REG(Add, "unsigned int Add(unsigned a,unsigned b)");

void main(void)
{
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    while(1)
    {
        if(SEGGER_RTT_HasKey()){
            Delay();
            Rx_size = SEGGER_RTT_HasData(0);
            SEGGER_RTT_Read(0, Rx_buf, Rx_size);
            Q_Sh_CmdHandler(1, (char *)Rx_buf);
            memset(Rx_buf, 0, BUF_LEN);
        }
    }
}
