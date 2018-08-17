#include "jx_i2c.h"

uint8_t wBuf[10], rBuf[300];

void main(void)
{
    I2C_InitTypeDef I2C_InitS;
    I2C_InitS.I2C_Mode = I2C_MASTER;
    I2C_InitS.I2C_Speed = I2C_FAST;
    I2C_InitS.I2C_AddrMode = I2C_7BIT;
    I2C_InitS.I2C_OwnAddress = 0x50;
    I2C_InitS.I2C_InterruptMask = 0;	//all interrupt mask
    I2C_Init(I2C0, &I2C_InitS);
    
    wBuf[0]=0x00;
    wBuf[1]=0x00;
    wBuf[2]=0x0A4;
    wBuf[3]=0x0B5;
    wBuf[4]=0x0C6;
    wBuf[5]=0x0D7;
    I2C_Write(I2C0, wBuf, 6);
    I2C_Write(I2C0, wBuf, 2);
    I2C_Read(I2C0,rBuf,4);
    while(1);
}