#include "jx_i2s.h"
#include "jx.h"

#ifdef MUSIC_11025
#define I2S_CLK_SRC     225792000
#else
#define I2S_CLK_SRC     20480000
#endif


void I2S_Mclk(I2S_InitTypeDef* I2S_InitStruct)
{
    uint32_t value;
    value = I2S_CLK_SRC/I2S_InitStruct->I2S_Mclk/10;
    register_write(0x3fe08024, 8, 24, 10);      //div part1
    register_write(0x3fe08024, 8, 16, value);   //div part2
}

void I2S_Sclk(I2S_InitTypeDef* I2S_InitStruct)
{
    uint32_t value,bit_ch;
    value = I2S_InitStruct->I2S_Mclk/I2S_InitStruct->I2S_Sclk;
    bit_ch = 24-8*(I2S_InitStruct->base_addr-I2S_BASE0)/0x1000;
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
    
    I2S_Sclk(I2S_InitStruct);
#endif
}

void I2S_Init(I2S_InitTypeDef* I2S_InitStruct)
{
    //debug_en disable
    if(I2S_BASE2 == I2S_InitStruct->base_addr){
        register_write(0x3fe0a000, 5, 0, 0x1F);     //pin_mask switch to I2S2
    }
    I2S_TCR(I2S_InitStruct->base_addr) = I2S_InitStruct->I2S_BitRlt;  //set as x bit resolution
    I2S_TFCR(I2S_InitStruct->base_addr) = 0x00; //Tx FIFO Configuration
    I2S_RFCR(I2S_InitStruct->base_addr) = 0x07; //Rx FIFO Configuration
    I2S_TER(I2S_InitStruct->base_addr) = 0x01;  //enable the ch0 tx
    I2S_RER(I2S_InitStruct->base_addr) = 0x01;  //enable the ch0 rx
    I2S_CCR(I2S_InitStruct->base_addr) = (I2S_InitStruct->I2S_ClkCyc<<3); //x clock cycles
    I2S_IER(I2S_InitStruct->base_addr) = 0x01;   //enable DW_apb_i2s
    I2S_ITER(I2S_InitStruct->base_addr) = 0x01;  //enable transmitter
    I2S_IRER(I2S_InitStruct->base_addr) = 0x01;  //enable receiver
    I2S_IMR(I2S_InitStruct->base_addr) = I2S_InitStruct->I2S_InterruptMask;     //unmark interrupt
    I2S_CER(I2S_InitStruct->base_addr) = 0x01;   //enable clock
    I2S_Clock(I2S_InitStruct);
}

void I2S_Write(I2S_InitTypeDef* I2S_InitStruct, uint32_t LValue, uint32_t RValue)
{
	I2S_LTHR(I2S_InitStruct->base_addr) = LValue & 0xFFFF;
	I2S_RTHR(I2S_InitStruct->base_addr) = RValue & 0xFFFF;
	while(!((I2S_ISR(I2S_InitStruct->base_addr))&0x10));
}
