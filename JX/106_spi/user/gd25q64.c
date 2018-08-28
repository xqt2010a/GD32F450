#include "gd25q64.h"

void SPIM1_Flash_Write_Enable(void)  //写使能
{
  	uint8_t cmd, r_temp;
	cmd = FLASH_WREN;
	SPIM1_Write_Read(&cmd, 1, &r_temp, 0);
}

void SPIM1_Flash_Write_Disable(void)  //关闭写使能
{
	uint8_t cmd, r_temp;
	cmd = FLASH_WRDI;
	SPIM1_Write_Read(&cmd, 1, &r_temp, 0);
}

uint32_t SPIM1_Flash_Read_ID(void)  //读 FlashID
{
	uint8_t cmd, r_temp[4];
	uint32_t FlashID;
	cmd = FLASH_RDID;
	SPIM1_Write_Read(&cmd, 1, r_temp, 4);
	FlashID = (r_temp[1]<<16)|(r_temp[2]<<8)|r_temp[3];
	return FlashID;
}

void SPIM1_Flash_Wait(void)  //等待写，擦完成
{
	uint8_t cmd, r_temp[2];
	cmd = FLASH_RSR1;	
	do
	{
		SPIM1_Write_Read(&cmd, 1, r_temp, 2); //读状态寄存器1
	}
	while((r_temp[1] & FLASH_WIP_FLAG) == FLASH_SET_WIP_FLAG);
}

void SPIM1_Flash_Page_Write(uint8_t *wBuffer, uint32_t WriteAddr, uint16_t N_Byte)  
{
	uint8_t w_temp[260], r_temp;  //256+4
	uint32_t i;
	SPIM1_Flash_Write_Enable();
  	w_temp[0] = FLASH_PP;
	w_temp[1] = ((WriteAddr & 0xFF0000) >> 16);
	w_temp[2] = ((WriteAddr & 0xFF00) >> 8);
	w_temp[3] = (WriteAddr & 0xFF);
	for(i=0; i<N_Byte; i++)
	{
		w_temp[4+i] = wBuffer[i];
	}
	
	SPIM1_Write_Read(w_temp, N_Byte+4, &r_temp, 0);
	SPIM1_Flash_Wait();  //等待写完成
}

void SPIM1_Flash_Page_Read(uint8_t *rBuffer, uint32_t ReadAddr, uint16_t N_Byte) //N_Byte <=256
{
	uint8_t  w_temp[4], r_temp[260];
	uint32_t i;
  	w_temp[0] = FLASH_READ;
	w_temp[1] = ((ReadAddr & 0xFF0000) >> 16);
	w_temp[2] = ((ReadAddr & 0xFF00) >> 8);
	w_temp[3] = (ReadAddr & 0xFF);
	
	SPIM1_Write_Read(w_temp, 4, r_temp, N_Byte+4);
	for(i=0; i<N_Byte; i++)
	{
		rBuffer[i] = r_temp[4+i];
	}
}

void SPIM1_Flash_Erase(uint8_t FlashCmd, uint32_t WriteAddr)  //擦除 Flash 
{
	uint8_t w_temp[4], r_temp;
	SPIM1_Flash_Write_Enable();
  	w_temp[0] = FlashCmd;
	w_temp[1] = ((WriteAddr & 0xFF0000) >> 16);
	w_temp[2] = ((WriteAddr & 0xFF00) >> 8);
	w_temp[3] = (WriteAddr & 0xFF);
	
	SPIM1_Write_Read(w_temp, 4, &r_temp, 0);
	SPIM1_Flash_Wait();  //等待擦完成
}