#include "jx_i2c.h"
#include "jx_uart.h"
#include "system.h"
#include "stdio.h"

#define I2C_N   I2C15//I2C4
#define LM75A_ADDR      0x48
#define AT24C32_ADDR    0xA0//0x50;0011010

uint8_t wBuf[10], rBuf[300];

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
    uint32_t sysclk;
    float value;
    
    smu_init1();
    
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("hello world!\r\n");
    
    JX_W4(0x3fe0a004) = 0x0;
    
    I2C_InitTypeDef I2C_InitS;
    I2C_InitS.I2C_Mode = I2C_MASTER;
    I2C_InitS.I2C_Speed = I2C_FAST;
    I2C_InitS.I2C_AddrMode = I2C_7BIT;
    I2C_InitS.I2C_OwnAddress = LM75A_ADDR;
    I2C_InitS.I2C_InterruptMask = 0;	//all interrupt mask
    I2C_Init(I2C_N, &I2C_InitS);
    
    if(LM75A_ADDR == I2C_InitS.I2C_OwnAddress){
        while(1){
            wBuf[0]=0x00;
            I2C_Write(I2C_N, wBuf, 1);
            I2C_Read(I2C_N,rBuf,2);
            value = ((rBuf[0]&0x7F)<<3)|(rBuf[1]>>5);
            printf("%4.2f¡æ\r\n", value/8);
            udelay(100000);
        }
    }
    else if(AT24C32_ADDR == I2C_InitS.I2C_OwnAddress){
        wBuf[0]=0x00;
        wBuf[1]=0xA1;
        wBuf[2]=0xA2;
        wBuf[3]=0xA3;
        wBuf[4]=0xA4;
        wBuf[5]=0xA5;
        I2C_Write(I2C_N, wBuf, 6);
        I2C_Write(I2C_N, wBuf, 1);
        I2C_Read(I2C_N,rBuf,4);
        rBuf[5] = rBuf[5];
        while(1);
    }
    else{
    
    }
}