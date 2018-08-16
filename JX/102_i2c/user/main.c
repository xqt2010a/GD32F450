#include "jx_i2c.h"

uint8_t wBuf[10], rBuf[300];

void main(void)
{
    I2C_InitTypeDef I2C_InitS;
    I2C_InitS.I2C_Mode = I2C_MASTER;
    I2C_InitS.I2C_Speed = I2C_FAST;
    I2C_InitS.I2C_AddrMode = I2C_7BIT;
    I2C_InitS.I2C_OwnAddress = 0x1A;
    I2C_InitS.I2C_InterruptMask = 0;	//all interrupt mask
    I2C_Init(I2C0, &I2C_InitS);
    
    wBuf[0]=0x30;
    wBuf[1]=0x01;
    I2C_Write(I2C0, wBuf, 2);
    while(1);
}