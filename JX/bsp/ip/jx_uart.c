#include "jx_uart.h"
#include "stdio.h"

void uart_init(unsigned int bd)
{
    UART_IER(UART0_BASE) = 0;
    while (UART_USR(UART0_BASE) & 0x1);
    UART_LCR(UART0_BASE) |= 0x80;  // the DLAB = 1
	UART_DLL(UART0_BASE)  = (uint8_t)(((UART_CLK/bd)/16)&0xFF);  // 115200=48M/16/0x1A
	UART_DLH(UART0_BASE)  = (uint8_t)(((((UART_CLK/bd)/16)&0xFF00)>>8)&0xFF);  // 00
	UART_LCR(UART0_BASE) &= (~0x80);  // the DLAB = 0
	UART_LCR(UART0_BASE) |= 0x03;  // none, 0, 8 
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");asm("nop");
    asm("nop");asm("nop");asm("nop");
}

void uart_tx(uint16_t Data)
{
	UART_THR(UART0_BASE) = Data;
	while((UART_LSR(UART0_BASE)) == 0);
}

uint16_t uart_rx(void)
{
	while(!(UART_LSR(UART0_BASE) & 0x01));
	return UART_RBR(UART0_BASE);
}

int fputc(int ch, FILE * f)
{
  	uart_tx(ch);
  	return ch;
}