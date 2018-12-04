#include "jx_uart.h"
#include "stdio.h"
#include "mmc_boot.h"
#include "system.h"


void main(void)
{
    uint32_t sysclk;
    sysclk = get_sysclk();
    uart_init(115200, sysclk);
    printf("mmc read write test!\r\n");
    mmc_test();
    while(1);
}