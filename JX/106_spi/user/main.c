#include "jx_spi.h"
#include "gd25q64.h"
#include "stdio.h"

#define LENTH  270
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
	//udelay(10000);
}

void main(void)
{
    uint32_t i, FlashID ;	
	uint8_t wBuf[LENTH];
	uint8_t rBuf[LENTH];
    
    smu_init1();
    SPIM1_Init();
    udelay(10000);
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