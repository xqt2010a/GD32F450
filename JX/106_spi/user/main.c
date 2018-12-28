#include "jx_spi.h"
#include "gd25q64.h"
#include "stdio.h"

#define LENTH  270

void main(void)
{
    uint32_t i, FlashID ;	
	uint8_t wBuf[LENTH];
	uint8_t rBuf[LENTH];
    
    SPIM1_Init();
    FlashID = SPI_Read_ID();
    for(i=0; i<LENTH; i++){
		wBuf[i] = 0xA5; //dummy byte
		rBuf[i] = 0;
	}
    
	FlashID = SPIM1_Flash_Read_ID();

	SPIM1_Flash_Page_Read(rBuf, 0, 6);

	SPIM1_Flash_Erase(FLASH_SE, 0x001030);
	
	SPIM1_Flash_Page_Read(rBuf, 0x001000, 256);
	
	for(i=0; i<1024; i++)
	{
	  	uint8_t j;
		j = i % 256;
		wBuf[i]=j;
	}
	SPIM1_Flash_Page_Write(wBuf, 0x001000, 256);
	
	SPIM1_Flash_Page_Read(rBuf, 0x001000, 256);
    FlashID = FlashID*2;
    while(FlashID);
}