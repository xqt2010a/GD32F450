#include "uart.h"
#include "stdio.h"

/* memory operation definition*/
#define REG32(addr)   		(*((volatile unsigned int *)(addr)))

#define UART_TIMEOUT 				(0x1ffff)

void uart_init(unsigned char parity, unsigned char stop_bits, unsigned char data_bits, unsigned int bd)
{

	unsigned int uart_ip_div;

	//sys_uart_pinmux(port, MUX_UART);

	// disable all interrupt
	REG32(UART_IER_REG) = 0x0;

	// set Rx Trigger Level & Tx FIFO Reset & Rx FIFO Reset & FIFO Mode Enable
	REG32(UART_FCR_REG) = UART_FCR_VAL(1, 1);

	// set Parity & Data bits & Stop bits
	REG32(UART_LCR_REG) = (UART_DLAB_EN<<7) | (parity<<3) | (stop_bits<<2) | data_bits;


	uart_ip_div = (50000000 + (16 * bd) / 2) / (16 * bd);


	REG32(UART_DLL_REG) = uart_ip_div & 0xFF;
	REG32(UART_DLH_REG) = (uart_ip_div >> 8) & 0xFF;

	REG32(UART_LCR_REG) = (UART_DLAB_DIS<<7) | (parity<<3) | (stop_bits<<2) | data_bits;

}


void uart_putc(unsigned char c)
{
	volatile unsigned int times = 0;

	while ((REG32(UART_LSR_REG) & LSR_THRE) == 0)
	{
		if (times++ >= UART_TIMEOUT)
		{
			return;
		}
	}

	REG32(UART_THR_REG) = c;
}


int fputc(int ch, FILE* stream)
{
    uart_putc((unsigned char)ch);
    return ch;
}