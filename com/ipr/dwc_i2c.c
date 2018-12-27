#include "dwc_i2c.h"

#define DUMMY_WRITE 0xA5

void dwc_i2c_init(I2C_InitTypeDef* i2c)
{
	I2C_ENABLE(i2c->ch) = 0x00;  // I2C disenable
	if(0 == i2c->mode)	//Master
	{
		I2C_CON(i2c->ch) = (1<<6)|(1<<5)|((i2c->addr_mode)<<4)
		  						|((i2c->speed+1)<<1)|(1);
	}
	else{						//Slave
		I2C_CON(i2c->ch) = (0<<6)|(1<<5)|((i2c->addr_mode)<<3)
		  						|((i2c->speed+1)<<1)|(0<<0);
	}
	I2C_TAR(i2c->ch) = i2c->own_addr;
	I2C_SS_SCL_HCNT(i2c->ch) = 0xAA;	// Set SCL High
	I2C_SS_SCL_LCNT(i2c->ch) = 0xAA;	// Set SCL Low
	I2C_RX_TL(i2c->ch) = 0;			//set Receive FIFO
	I2C_TX_TL(i2c->ch) = 0x2;		//set transmit FIFO
	I2C_INTR_MASK(i2c->ch) = i2c->int_mask; //Clear the interrupt mask
	I2C_ENABLE(i2c->ch) = 0x01;   // I2C enable
}

void dwc_i2c_write(I2C_InitTypeDef* i2c, unsigned char * wBuf, unsigned char len)
{
	unsigned char i;
	
  	for(i=0; i<(len-1); i++)
	{
		I2C_DATA_CMD(i2c->ch) = wBuf[i]&0x0FF;
	}
	I2C_DATA_CMD(i2c->ch) = (wBuf[i]&0x0FF)|(1<<9);
}

void dwc_i2c_read(I2C_InitTypeDef* i2c, unsigned char * rBuf, unsigned char len)
{
	unsigned char i;
	
	for(i=0; i<(len-1); i++)
	{
		I2C_DATA_CMD(i2c->ch) = DUMMY_WRITE|(1<<8);
		
	}
	I2C_DATA_CMD(i2c->ch) = DUMMY_WRITE|(1<<8)|(1<<9);  //read & stop
	
	i=0;
	while(i<(len))
	{
	  	while((I2C_STATUS(i2c->ch)&(1<<3)))
		{
			rBuf[i] = (unsigned char)(I2C_DATA_CMD(i2c->ch) & 0x0FF);
			i++;
			//len -= 1;
		}
	}
}