#include "jx_i2s.h"
#include "jx.h"

#ifdef MUSIC_11025
#define I2S_CLK_SRC     225792000
#else
#define I2S_CLK_SRC     20480000
#endif


void I2S_Mclk(uint32_t mclk)
{
    uint32_t value;
    value = I2S_CLK_SRC/mclk/10;
    register_write(0x3fe08024, 8, 24, 10);      //div part1
    register_write(0x3fe08024, 8, 16, value);   //div part2
}

void I2S_Sclk(uint32_t mclk, uint32_t sclk)
{
    uint32_t value;
    value = mclk/sclk;
    register_write(0x3fe08028, 8, 24, value);
}

void I2S_Clock(uint32_t sample_clk, uint8_t sample_bit)
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
    
    I2S_Mclk(sample_clk*256);
    I2S_Sclk(sample_clk*256, sample_clk*2*sample_bit);
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
    
    I2S_Sclk(sample_clk*256, sample_clk*2*sample_bit);
#endif
}

void I2S_Init(I2S_InitTypeDef* I2S_InitStruct)
{
	I2S_TCR0 = I2S_InitStruct->I2S_BitRlt;  //set as x bit resolution
	I2S_TFCR0 = 0x00; //Tx FIFO Configuration
	I2S_RFCR0 = 0x07; //Rx FIFO Configuration
	I2S_TER0 = 0x01;  //enable the ch0 tx
	I2S_RER0 = 0x01;  //enable the ch0 rx
	I2S_CCR = (I2S_InitStruct->I2S_ClkCyc<<3); //x clock cycles
	I2S_IER = 0x01;   //enable DW_apb_i2s
	I2S_ITER = 0x01;  //enable transmitter
	I2S_IRER = 0x01;  //enable receiver
	I2S_IMR0 = I2S_InitStruct->I2S_InterruptMask;     //unmark interrupt
	I2S_CER = 0x01;   //enable clock
}

void I2S_Write(uint32_t LValue, uint32_t RValue)
{
	I2S_LTHR0 = LValue & 0xFFFF;
	I2S_RTHR0 = RValue & 0xFFFF;
	while(!((I2S_ISR0)&0x10));
}
