/*
 * I2C.c
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: cnan
 */
#include "jx_i2c.h"
#include "jx.h"

void I2C_Init(unsigned char ch, I2C_InitTypeDef* I2C_InitS)
{
        //JX_WR4(0x3fe0a004) = 0x0;
	I2C_ENABLE(ch) = 0x00;  // I2C disenable
	if(0 == I2C_InitS->I2C_Mode)	//Master
	{
		I2C_CON(ch) = (1<<6)|(1<<5)|((I2C_InitS->I2C_AddrMode)<<4)
		  						|((I2C_InitS->I2C_Speed+1)<<1)|(1);
	}
	else{						//Slave
		I2C_CON(ch) = (0<<6)|(1<<5)|((I2C_InitS->I2C_AddrMode)<<3)
		  						|((I2C_InitS->I2C_Speed+1)<<1)|(0<<0);
	}
	I2C_TAR(ch) = I2C_InitS->I2C_OwnAddress;
	I2C_SS_SCL_HCNT(ch) = 0xAA;	// Set SCL High
	I2C_SS_SCL_LCNT(ch) = 0xAA;	// Set SCL Low
	I2C_RX_TL(ch) = 0;			//set Receive FIFO
	I2C_TX_TL(ch) = 0x2;		//set transmit FIFO
	I2C_INTR_MASK(ch) = I2C_InitS->I2C_InterruptMask; //Clear the interrupt mask
	I2C_ENABLE(ch) = 0x01;   // I2C enable
}

void I2C_Write(unsigned char ch, unsigned char * wBuf, unsigned char len)
{
	unsigned char i;
	
  	for(i=0; i<(len-1); i++)
	{
		I2C_DATA_CMD(ch) = wBuf[i]&0x0FF;
	}
	I2C_DATA_CMD(ch) = (wBuf[i]&0x0FF)|(1<<9);
}

void I2C_Read(unsigned char ch, unsigned char * rBuf, unsigned char len)
{
	unsigned char i;
	
	for(i=0; i<(len-1); i++)
	{
		I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8);
		
	}
	I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8)|(1<<9);  //read & stop
	
	i=0;
	while(i<(len))
	{
	  	while((I2C_STATUS(ch)&(1<<3)))
		{
			rBuf[i] = (unsigned char)(I2C_DATA_CMD(ch) & 0x0FF);
			i++;
			//len -= 1;
		}
	}
}


uint16_t read_lm75a(unsigned char ch)
{
    uint16_t r_value, i, buf[4];
    I2C_DATA_CMD(ch) = (0x00&0x0FF)|(1<<9);;    //read temp
    I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8);
    I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8)|(1<<9);  //read & stop
    i=0;
	while(i<2){
	  	while((I2C_STATUS(ch)&(1<<3))){
			buf[i] = (unsigned char)(I2C_DATA_CMD(ch) & 0x0FF);
			i++;
		}
	}
    r_value = ((buf[1]&0x7F)<<3)|(buf[2]>>5);
    return r_value;
}
/*
void main(void)
{
	uint8_t wBuf[10], rBuf[300];
	uint32_t i;
	
	for(i=0; i<300; i++)
	{
		rBuf[i] = 0;
	}
	
	wBuf[0] = 0;
	wBuf[1] = 1;
	wBuf[2] = 2;
	wBuf[3] = 3;
	
	IIC_InitTypeDef IIC_InitS;
	IIC_InitS.IIC_Mode = IIC_MASTER;
	IIC_InitS.IIC_Speed = IIC_STANDARD;
	IIC_InitS.IIC_AddrMode = IIC_7BIT;
	IIC_InitS.IIC_OwnAddress = ;
	IIC_InitS.IIC_InterruptMask = 0;	//all interrupt mask
	IIC_Init(IIC0, &IIC_InitS);
	
	//I2C_Write(I2C1_BASE, wBuf, 4);
		
	for(i=0; i<0xFFFF; i++);
	
	I2C_Write(I2C1_BASE, wBuf, 2);
	I2C_Read(I2C1_BASE, rBuf, 8);
	
	for(i=0; i<0xFFFF; i++);
}
*/


