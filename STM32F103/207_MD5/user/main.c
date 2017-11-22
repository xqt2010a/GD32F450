#include "SEGGER_RTT.h"
#include "string.h"
#include "stdint.h"
#include "stdio.h"
#include "md5.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

unsigned char encrypt[] ="admin";//21232f297a57a5a743894a0e4a801fc3 
//unsigned char decrypt[16]; 

void main(void)
{
    uint8_t i;
    MD5_CTX md5;
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    PRT("hello world!\r\ntoday is :%s,%s\r\n",__DATE__, __TIME__);
    
    MD5Init(&md5);
    while(1)
    {
        MD5Update(&md5,encrypt,strlen((char *)encrypt));  
        MD5Final(&md5); 
        printf("entrypt:%s\r\ndecrypt:",encrypt);
        for(i=0;i<16;i++)  
        {  
            printf("%02x",md5.digest[i]);  
        }
        while(1);
    }
}
