/**********************************************************************************
* 文件名  ：spi.c
* 描述    ：spi 驱动函数库         
* 编写	   ：Labc
* 时间    ：2016-04-13
**********************************************************************************/
#include "spi.h"

void SPI_Init(void)
{
#if (1 == SPI_HARDWARE)
    
#else
    P1DIR = (1<<3)|( 1<< 4) | (1<<5 ) | (1<<6 ) | (0<<7 );
    SPI_CS = 1;
    SPI_CLK = 0;  //需要在 CS前拉低
    SPI_RST = 1;
#endif	
}

unsigned char SPI_ReadByte(void)
{
#if (1 == SPI_HARDWARE)
    
#else
    unsigned char i,data;
    
    data = 0;
    SPI_CLK = 0;
    
    for(i=0; i<8; i++)
    {
        SPI_CLK = 1;
        data <<= 1;
        if(SPI_MISO != 0)
            data |= 0x01;
        SPI_CLK = 0;
    }
    SPI_CLK = 1;
	return data;
#endif
}

void SPI_WriteByte(unsigned char data)
{
#if (1 == SPI_HARDWARE)
    
#else
    unsigned char i;
    
    SPI_CLK = 0;
    
    for(i=0; i<8; i++)
    {
        if(0x80 == (data & 0x80))
            SPI_MOSI = 1;
        else
            SPI_MOSI = 0;
        SPI_CLK = 1;
        data <<= 1;
        SPI_CLK = 0;
    }
    SPI_CLK = 0;
#endif	
}


//************************************************************************
/*********************************************************************************************************
** Function name:       spi_SetReg
** Descriptions:        SPID′?á?¨D?????′??÷oˉêy
** input parameters:    ucRegAddr￡o??′??÷μ??·
**                      ucRegVal￡oòaD′è?μ??μ
** output parameters:   ?T
** Returned value:      TRUE
*********************************************************************************************************/
uint8_t spi_SetReg(uint8_t ucRegAddr, uint8_t ucRegVal)
{
    uint8_t i, ucAddr;
    
    SPI_CLK = 0;
    SPI_CS = 0;
    ucAddr = ((ucRegAddr<<1)&0x7E);
    
    for(i=8;i>0;i--)
    {
        if((ucAddr&0x80)==0x80)
        {
            SPI_MOSI = 1;
        }else
        {
            SPI_MOSI = 0;
        }
        SPI_CLK = 1;
        ucAddr <<= 1;
        SPI_CLK = 0;
    }
    
    for(i=8;i>0;i--)
    {
        if((ucRegVal&0x80)==0x80)
        {
            SPI_MOSI = 1;
        }else
        {
            SPI_MOSI = 0;
        }
        SPI_CLK = 1;
        ucRegVal <<= 1;
        SPI_CLK = 0;
    }
    
    SPI_CS = 1;
    SPI_CLK = 1;
    
	return TRUE;
}

/*********************************************************************************************************
** Function name:       spi_GetReg
** Descriptions:        SPI?á?á?¨D?????′??÷oˉêy
** input parameters:    ucRegAddr￡o??′??÷μ??·
** output parameters:   ?T
** Returned value:      ??±ê??′??÷μ??μ
*********************************************************************************************************/
uint8_t spi_GetReg(uint8_t ucRegAddr)
{ 
    uint8_t i, ucAddr;
    uint8_t ucRegVal=0;
    
    SPI_CLK = 0;
    SPI_CS = 0;
    ucAddr = ((ucRegAddr<<1)&0x7E)|0x80;
    
    for(i=8;i>0;i--)
    {
        if((ucAddr&0x80)==0x80)
        {
            SPI_MOSI = 1;
        }else
        {
            SPI_MOSI = 0;
        }
        SPI_CLK = 1;
        ucAddr <<= 1;
        SPI_CLK = 0;
    }
    
    for(i=8;i>0;i--)
    {
        SPI_CLK = 1;
        ucRegVal <<= 1;
        if(SPI_MISO != 0)
        {
            ucRegVal |= 0x01;
        }
        SPI_CLK = 0;
    }
    
    SPI_CS = 1;
    SPI_CLK = 1;
	
    return ucRegVal;
}

/*********************************************************************************************************
** Function name:       spi_GetReg
** Descriptions:        SPI?á?á?¨D?????′??÷oˉêy
** input parameters:    ucRegAddr￡o??′??÷μ??·; uint8_t *p êy?Y′?·?μ??·
** output parameters:   ?T
** Returned value:      ??±ê??′??÷μ??μ
*********************************************************************************************************/
uint8_t spi_GetReg2(uint8_t ucRegAddr, uint8_t *p)
{
    uint8_t i, ucAddr;
    
    SPI_CLK = 0;
    SPI_CS = 0;
    ucAddr = ((ucRegAddr<<1)&0x7E)|0x80;
    
    for(i=8;i>0;i--)
    {
        if((ucAddr&0x80)==0x80)
            SPI_MOSI = 1;
        else
            SPI_MOSI = 0;
        SPI_CLK = 1;
        ucAddr <<= 1;
        SPI_CLK = 0;
    }
    
    for(i=8;i>0;i--)
    {
        SPI_CLK = 1;
        *p <<= 1;
        if(SPI_MISO != 0)
            *p |= 0x01;
        SPI_CLK = 0;
    }
    
    SPI_CS = 1;
    SPI_CLK = 1;
    
    return TRUE;
}

