#include "dwc_i2s.h"

void dwc_init(I2S_InitTypeDef * i2s)
{
    I2S_TCR(i2s->ch) = i2s->bit_rlt;  //set as x bit resolution
    I2S_TFCR(i2s->ch) = 0x00; //Tx FIFO Configuration
    I2S_RFCR(i2s->ch) = 0x07; //Rx FIFO Configuration
    I2S_TER(i2s->ch) = 0x01;  //enable the ch0 tx
    I2S_RER(i2s->ch) = 0x01;  //enable the ch0 rx
    I2S_CCR(i2s->ch) = (i2s->clk_cyc<<3); //x clock cycles
    I2S_IER(i2s->ch) = 0x01;   //enable DW_apb_i2s
    I2S_ITER(i2s->ch) = 0x01;  //enable transmitter
    I2S_IRER(i2s->ch) = 0x01;  //enable receiver
    I2S_IMR(i2s->ch) = i2s->int_mask;     //unmark interrupt
    I2S_CER(i2s->ch) = 0x01;   //enable clock
}

void dwc_i2s_write(I2S_InitTypeDef * i2s, uint32_t LValue, uint32_t RValue)
{
	I2S_LTHR(i2s->ch) = LValue & 0xFFFF;
	I2S_RTHR(i2s->ch) = RValue & 0xFFFF;
	while(!((I2S_ISR(i2s->ch))&0x10));
}