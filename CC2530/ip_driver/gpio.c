#include "gpio.h"

void GPIO_Init(void)
{
    //P1DIR |= ( 1<< 0) | (1<<1 ) | (0<<2 ) | (1<<3 ) | (1<<4 ) | (0<<5 );  
	P1DIR = ( 1<< 0) | (1<<1 ) | (0<<2 ) | (1<<3 ) | (1<<7 ) | (0<<5 ); 
	SPI_CS = 1;
	SPI_SCLK = 1;
	SPI_MOSI = 1;
	SPI_RST = 1;
	
    LED1 = 0; 
	LED2 = 0;
}

void LED1_On(void)
{
	LED1 = 1;
}

void LED1_Off(void)
{
	LED1 = 0;
}

void LED2_On(void)
{
	LED2 = 1;
}

void LED2_Off(void)
{
	LED2 = 0;
}