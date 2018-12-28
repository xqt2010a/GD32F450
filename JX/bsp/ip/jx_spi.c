#include "jx_spi.h"

#define DUMMY_BYTE  0xA5

void SPI_Init(SPI_InitTypeDef * spi)
{
    (*(volatile unsigned long*)(0x3fe09004)) = 0xff000000;  //add
  	dwc_spi_init(spi);
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
    unsigned char i;
    
    uint8_t rFifo[4]={0};
    SSIM1_DR = 0x9F;
    SSIM1_DR = 0xA5;
    SSIM1_DR = 0xA5;
    SSIM1_DR = 0xA5;
    while(0 == SSIM1_RXFLR);
    for(i=0; i<4; i++){
        while(!((SSIM1_SR & 0x8)>>3));
        rFifo[i] = SSIM1_DR;
    }
    return ((rFifo[1]<<16)|(rFifo[1]<<8)|(rFifo[1]));
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
