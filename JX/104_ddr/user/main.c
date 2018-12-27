#include "stdio.h"
#include "ddr.h"
#include "ddr_test.h"
#include "uart.h"
#include "lpddr4_init_training_pass.h"

void main(void)
{
    set_ddrc_freq(800); //before other device setting
    uart_init(UART_PARITY_NONE, UART_STOPBITS_1, UART_DATABITS_8, UART_BD_115200);
    printf("start ddr init\n");
    ddr_init();

    ddr_order();
    ddr_rand();
    while(1);
}