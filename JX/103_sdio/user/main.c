#include "jx_uart.h"
#include "stdio.h"
#include "mmc_boot.h"


void main(void)
{
    uart_init(115200);
    printf("mmc read write test!\r\n");
    mmc_test();
    while(1);
}