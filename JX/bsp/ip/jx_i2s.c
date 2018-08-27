#include "jx_i2s.h"

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
