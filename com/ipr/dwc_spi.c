#include "dwc_spi.h"

void dwc_spi_init(SPI_InitTypeDef * spi)
{
    SPI_SSIENR(spi->ch) = 0;  //���� SSI �Ĵ���
    SPI_CTRLR0(spi->ch) = 0x07 |(1<<6)|(1<<7);  //���ͺͽ��գ�ģʽ0��Motorola SPI��8λ�������ݴ���
    SPI_BAUDR(spi->ch) = 0xF0;  //Fsclk_out / SCKDV = Fssi_clk  0x500,0x30//40M 0x04 
	//RXFTLR = 0x20;
	SPI_SER(spi->ch) = 0x01;  //������ʹ�ܼĴ��� 
	SPI_SSIENR(spi->ch) = 1;  //ʹ�� SSI �Ĵ���
}

