#include "jx_spi.h"

#define DUMMY_BYTE  0xA5

void SPIM1_Init(void)
{
    (*(volatile unsigned long*)(0x3fe09004)) = 0xff000000;  //add
  	//SPIM1 时钟 80M
    SSIM1_SSIENR = 0;  //禁用 SSI 寄存器
    SSIM1_CTRLR0 = 0x07 |(1<<6)|(1<<7);  //发送和接收，模式0，Motorola SPI，8位串行数据传输
    SSIM1_BAUDR = 0xF0;  //Fsclk_out / SCKDV = Fssi_clk  0x500,0x30//40M 0x04 
	//RXFTLR = 0x20;
	SSIM1_SER = 0x01;  //从器件使能寄存器 
	SSIM1_SSIENR = 1;  //使能 SSI 寄存器  
}

void SPIM1_Write_Read_Fifo(uint8_t * wFifo, uint8_t * rFifo, uint32_t N_Byte)
{
	uint32_t i;
	for(i=0; i<N_Byte; i++)
	{
		SSIM1_DR = wFifo[i];
		while(0 == SSIM1_RXFLR);
		rFifo[i] = SSIM1_DR;	
	}
}

int SPI_Read_ID(void)
{
//    unsigned int data;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x08)) = 0;
//    (*(volatile unsigned long*)(0x3fe09004)) = 0xff000000;  //add
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x00)) = 0x011f0100;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x04)) = 8;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x14)) = 4;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x18)) = 4;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x1c)) = 4;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x2c)) = 0x1f;    
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x10)) = 1;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x08)) = 1;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x18)) = 4;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x1c)) = 4;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x2c)) = 0x1f;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x60)) = 0x14131211;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x60)) = 0x18171615;
//    (*(volatile unsigned long*)(SSIM1_BASE + 0x60)) = 0x1c1b1a19;
    
    uint8_t rFifo[4]={0};
    SSIM1_DR = 0x9F;
    SSIM1_DR = 0xA5;
    SSIM1_DR = 0xA5;
    SSIM1_DR = 0xA5;
    while(0 == SSIM1_RXFLR);
    rFifo[0] = SSIM1_DR;
    rFifo[1] = SSIM1_DR;
    rFifo[2] = SSIM1_DR;
    rFifo[3] = SSIM1_DR;
    return rFifo[0];
}

void SPIM1_Write_Read(uint8_t * wBuf, uint32_t wLen, uint8_t * rBuf, uint32_t rLen)
{
  	uint8_t temp;
  	uint32_t i;
	if(wLen > rLen)  
	{
		SSIM1_DR = wBuf[0];
		SPIM1_Write_Read_Fifo((wBuf+1), rBuf, rLen);
		for(i=0; i<(wLen - rLen - 1); i++)
		{
			SSIM1_DR = wBuf[rLen+1+i];
			while(0 == SSIM1_RXFLR);
			temp = SSIM1_DR;
		}
		while(0 == SSIM1_RXFLR);
		temp = SSIM1_DR;
	}
	else if(wLen == rLen) //wLen and rLen can not be 0 at the same time
	{
		SSIM1_DR = wBuf[0];
		SPIM1_Write_Read_Fifo((wBuf+1), rBuf, (wLen-1));
		while(0 == SSIM1_RXFLR);
		rBuf[rLen-1] = SSIM1_DR;
	}
	else  //wLen < rLen
	{
		if(0 != wLen)
		{
			SSIM1_DR = wBuf[0];
			SPIM1_Write_Read_Fifo((wBuf+1), rBuf, (wLen-1));
			for(i=0; i<(rLen-wLen); i++)
			{
				SSIM1_DR = DUMMY_BYTE;
                temp = SSIM1_RXFLR;
				while(0 == temp);
				rBuf[wLen-1+i] = SSIM1_DR;
			}
			while(0 == SSIM1_RXFLR);
			rBuf[rLen-1] = SSIM1_DR;
		}
		else  //wLen == 0
		{
			SSIM1_DR = DUMMY_BYTE;
			for(i=0; i<(rLen-1); i++)
			{
				SSIM1_DR = DUMMY_BYTE;
				while(0 == SSIM1_RXFLR);
				rBuf[i] = SSIM1_DR;
			}
			while(0 == SSIM1_RXFLR);
			rBuf[rLen-1] = SSIM1_DR;
		}		
	}
    temp = temp;
}
