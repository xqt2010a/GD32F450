#include "jx_uart.h"
#include "jx_i2s.h"
#include "jx_i2c.h"
#include "system.h"
#include "wm8731.h"
#ifdef MUSIC_11025
#include "music_wav.h"
#else
#include "8k.h"
#endif
#include "stdio.h"

UART_InitTypeDef uart;

void uart_struct_init(void)
{
    uart.ch = UART1;
    uart.int_mask = 0;
    uart.bandrate = 115200;
}

void main(void)
{
    uint32_t i;
    I2S_InitTypeDef i2s;

    /* EVB2.0
    * I2S2 --- I2C0
    * I2S1 --- I2C2
    * I2S0 --- I2C1
    */
    i2s.ch = I2S1;
    i2s.bit_rlt = 2;
    i2s.clk_cyc = 0;
#ifdef MUSIC_11025
    i2s.sclk = 11025*2*16;
    i2s.mclk = 11025*256;
#else
    i2s.sclk = 8000*2*16;
    i2s.mclk = 8000*256;
#endif    
    uart_struct_init();
    uart_init(&uart);
    I2S_Init(&i2s);
    wm8731_init();
    wm8731_set_headphone_volume(WM8731_DIR_RIGHT, WM8731_MODE_HIGH, 0x70);
    
    printf("hello world!\r\n");
    while(1){
        printf("The headphone is read \"Enter\".\r\n");
#ifdef MUSIC_11025        
        for (i = 0; i < 4500; i++)
#else
        for (i = 0; i < 16000; i++)
#endif
        {
            dwc_i2s_write(&i2s, wav_temp[i], wav_temp[i]);
        }
    }
}