#include "jx_i2s.h"
#include "jx.h"

#ifdef MUSIC_11025
#define I2S_CLK_SRC     225792000
#else
#define I2S_CLK_SRC     20480000
#endif


void I2S_Mclk(I2S_InitTypeDef* i2s)
{
    uint32_t value;
    value = I2S_CLK_SRC/i2s->mclk/10;
    register_write(0x3fe08024, 8, 24, 10);      //div part1
    register_write(0x3fe08024, 8, 16, value);   //div part2
}

void I2S_Sclk(I2S_InitTypeDef* i2s)
{
    uint32_t value,bit_ch;
    value = i2s->mclk/i2s->sclk;
    bit_ch = 24-8*i2s->ch;
    register_write(0x3fe08028, 8, bit_ch, value);
}

void I2S_Clock(I2S_InitTypeDef* I2S_InitStruct)
{   
#ifdef MUSIC_11025
    register_write(0x3fe0802c, 1, 30, 0);       //Switch PLL clkmux to select OSC 24MHz
    register_write(0x3fe0801c, 8, 0, 0xf0);     //Power down PLL
    register_write(0x3fe0801c, 6, 26, 25);       //1 25
    register_write(0x3fe0801c, 12, 14, 441*8);     //64 441*8
    register_write(0x3fe0801c, 3, 11, 5);       //5 5
    register_write(0x3fe0801c, 3, 8, 3);        //5 3
    register_write(0x3fe0801c, 8, 0, 0x00);     //Power on PLL  24M/1*64/5/5=61.44M
    while(1 != register_read(0x3fe08080, 1, 30));       //polling pll
    register_write(0x3fe0802c, 1, 30, 1);       //Switch PLL clkmux to select PLL output
    
    I2S_Mclk(I2S_InitStruct);
    I2S_Sclk(I2S_InitStruct);
#else    
    register_write(0x3fe0802c, 1, 30, 0);       //Switch PLL clkmux to select OSC 24MHz
    register_write(0x3fe0801c, 8, 0, 0xf0);     //Power down PLL
    register_write(0x3fe0801c, 6, 26, 3);       //1 25
    register_write(0x3fe0801c, 12, 14, 64);     //64 441*8
    register_write(0x3fe0801c, 3, 11, 5);       //5 5
    register_write(0x3fe0801c, 3, 8, 5);        //5 3
    register_write(0x3fe0801c, 8, 0, 0x00);     //Power on PLL  24M/1*64/5/5=61.44M
    while(1 != register_read(0x3fe08080, 1, 30));       //polling pll
    register_write(0x3fe0802c, 1, 30, 1);       //Switch PLL clkmux to select PLL output
    
    I2S_Sclk(i2s);
#endif
}

void I2S_Init(I2S_InitTypeDef* i2s)
{
    //debug_en disable
    if(I2S2 == i2s->ch){
        register_write(0x3fe0a000, 5, 0, 0x1F);     //pin_mask switch to I2S2
    }
    dwc_init(i2s);
    I2S_Clock(i2s);
}