#include "SEGGER_RTT.h"
#include "string.h"
//#include "led.h"
#include "DAP.h"

#define BUF_LEN     128

uint8_t Req[1024] = {0x00,0x00};
uint8_t Rep[1024];

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

void main(void)
{
    int c;
    uint32_t i, j, temp, num;
    uint8_t rx_buf[128];
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    DAP_Setup();                          // DAP Setup 
    //Led_Init();
    
    while(1)
    {
        //Led_Blink();
        memset(Req, 0, BUF_LEN);
        c = SEGGER_RTT_HasKey();
        if(1 == c)
        {
            for(i=0; i<0xFFFF; i++);  /* delay */
            c = SEGGER_RTT_HasData(0);
            SEGGER_RTT_Read(0, rx_buf, c);
            
            for(j=0; j<(c-2); j++)
            {
                temp = (rx_buf[j]-0x30)<<4;
                j++;
                Req[j/3] = (rx_buf[j]-0x30) + temp;
                j++;
                PRT("0x%X ",Req[j/3]);
            }
            
                
            num = DAP_ProcessCommand(Req, Rep);
            num = num&0xFFFF;
            PRT("\r\nRep:num:%d\r\nRep_buf:",num);
            if(num < 128){
                for(j=0; j<num; j++){
                    PRT("%2X ",Rep[j]);
                }
                PRT("\r\n");
            }
            else{
                PRT("Num Error!\r\n");
            }
        }
        
    }
}
