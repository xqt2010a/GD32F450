#include "jx_i2c.h"

uint8_t wBuf[10], rBuf[300];

#define JX_W4(x)    (*(unsigned int *)(x))

void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void smu_init(void)
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
    smu_init();
    
    JX_W4(0x3fe0a004) = 0x0;
    
    I2C_InitTypeDef I2C_InitS;
    I2C_InitS.I2C_Mode = I2C_MASTER;
    I2C_InitS.I2C_Speed = I2C_FAST;
    I2C_InitS.I2C_AddrMode = I2C_7BIT;
    I2C_InitS.I2C_OwnAddress = 0x50;//;0x1A;//0x50;0011010
    I2C_InitS.I2C_InterruptMask = 0;	//all interrupt mask
    I2C_Init(I2C4, &I2C_InitS);
    
    wBuf[0]=0x30;
    wBuf[1]=0x00;
    wBuf[2]=0x12;
    wBuf[3]=0x0B5;
    wBuf[4]=0x0C6;
    wBuf[5]=0x0D7;
    I2C_Write(I2C4, wBuf, 3);
    I2C_Write(I2C4, wBuf, 2);
    I2C_Read(I2C4,rBuf,4);
    rBuf[5] = rBuf[5];
    while(1);
}