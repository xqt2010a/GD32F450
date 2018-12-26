#include "jx_uart.h"
#include "stdio.h"
#include "stdint.h"

static uint8_t uart_buf_rx[32]={0};

void uart_init(uint32_t bd, uint32_t clk)
{
    
    UART_IER(UART_BASE) = 0;
    while (UART_USR(UART_BASE) & 0x1);
    UART_LCR(UART_BASE) |= 0x80;    // the DLAB = 1
    UART_DLL(UART_BASE)  = (uint8_t)(((clk/bd)/16)&0xFF);                  // 115200=48M/16/0x1A
    UART_DLH(UART_BASE)  = (uint8_t)(((((clk/bd)/16)&0xFF00)>>8)&0xFF);    // 00
    UART_LCR(UART_BASE) &= (~0x80); // the DLAB = 0
    UART_LCR(UART_BASE) |= 0x03;    // none, 0, 8 
    UART_IER(UART_BASE) |= 5;       // enable the rx interrupt
}

void uart_tx(uint16_t Data)
{
    UART_THR(UART_BASE) = Data;
    while((UART_LSR(UART_BASE)) == 0);
}

uint8_t uart_rx(void)
{
    while(!(UART_LSR(UART_BASE) & 0x01));
    return UART_RBR(UART_BASE);
}

int fputc(int ch, FILE * f)
{
  	uart_tx(ch);
  	return ch;
}

void uart_irq(void)
{
    static uint32_t i=0;
    uart_buf_rx[i++] = uart_rx();
    uart_buf_rx[0] = uart_buf_rx[0];
}