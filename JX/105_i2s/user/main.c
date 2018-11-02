#include "jx_uart.h"
#include "jx_i2s.h"
#include "jx_i2c.h"
#include "system.h"
#include "wm8731.h"
#include "music_wav.h"
//#include "8k.h"
#include "stdio.h"

#define JX_W4(x)    (*(unsigned int *)(x))

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init1(void)
{
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08100) = 0xFFFFFFFF;
    JX_W4(0x3FE08104) = 0xFFFFFFFF;
    JX_W4(0x0190d100) = 0xFFFFFFFF;
    JX_W4(0x0190d104) = 0xFFFFFFFF;
    JX_W4(0x0190d108) = 0xFFFFFFFF;
    JX_W4(0x0190d10c) = 0xFFFFFFFF;
    JX_W4(0x0190d110) = 0xFFFFFFFF;
    JX_W4(0x0190d114) = 0xFFFFFFFF;
    JX_W4(0x0190d120) = 0xFFFFFFFF;
    JX_W4(0x0190d124) = 0xFFFFFFFF;
    
    /* Delay for somewhile to wait reset de-assertion to be stable. */
    udelay(10000);
}

void main(void)
{
    uint32_t sysclk, i;
    I2S_InitTypeDef I2S_InitS;
    
    I2S_InitS.I2S_BitRlt = 2;
    I2S_InitS.I2S_ClkCyc = 0;
    
    smu_init1();
    I2S_Clock(11025, 16);
    //I2S_Clock(8000, 16);
    I2S_Init(&I2S_InitS);
    wm8731_init();
    wm8731_set_headphone_volume(WM8731_DIR_RIGHT, WM8731_MODE_HIGH, 0x70);
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\n");
    while(1){
        printf("The headphone is read \"Enter\".\r\n");
        
        for (i = 0; i < 4500; i++)
        //for (i = 0; i < 16000; i++)
        {
            I2S_Write(wav_temp[i], wav_temp[i]);
        }
    }
}