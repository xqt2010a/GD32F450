#include "dwc_uart.h"

uint8_t uart_ch = 0;

void dwc_uart_init(UART_InitTypeDef *uart)
{
    UART_IER(uart->ch) = 0;
    while (UART_USR(uart->ch) & 0x1);
    UART_LCR(uart->ch) |= 0x80;    // the DLAB = 1
    UART_DLL(uart->ch)  = (uint8_t)(((uart->clk / uart->bandrate)/16)&0xFF);  // 115200=48M/16/0x1A
    UART_DLH(uart->ch)  = (uint8_t)(((((uart->clk / uart->bandrate)/16)&0xFF00)>>8)&0xFF);    // 00
    UART_LCR(uart->ch) &= (~0x80); // the DLAB = 0
    UART_LCR(uart->ch) |= 0x03;    // none, 0, 8
    UART_IER(uart->ch) = uart->int_mask;
    uart_ch = uart->ch;
}

void dwc_uart_tx(UART_InitTypeDef *uart, uint16_t data)
{
    UART_THR(uart->ch) = data;
    while((UART_LSR(uart->ch)) == 0);
}

uint8_t dwc_uart_rx(UART_InitTypeDef *uart)
{
    while(!(UART_LSR(uart->ch) & 0x01));
    return UART_RBR(uart->ch);
}

int fputc(int ch, FILE * f)
{
    UART_THR(uart_ch) = ch;
    while((UART_LSR(uart_ch)) == 0);
    return ch;
}