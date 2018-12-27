#include "jx_i2c.h"
#include "jx_uart.h"
#include "system.h"
#include "stdio.h"

#define LM75A_ADDR      0x48
#define AT24C32_ADDR    0xA0//0x50;0011010

I2C_InitTypeDef i2c;
UART_InitTypeDef uart;

uint8_t wBuf[10], rBuf[300];

static void udelay(unsigned int t)
{
    unsigned int i,j;
    for(i=0; i<t; i++){
        for(j=0; j<100; j++);
    }
}

void i2c_struct_init(void)
{
    i2c.ch = I2C0;
    i2c.mode = 0;       //0:master 1:salve
    i2c.addr_mode = 0;  //0:7bit 1:10bit
    i2c.int_mask = 0;   //see datasheet
    i2c.speed = 0;      //0:stand 1:fash 2:high
    i2c.clk = 0;
    i2c.own_addr = LM75A_ADDR;
}

void uart_struct_init(void)
{
    uart.ch = UART1;
    uart.int_mask = 0;
    uart.bandrate = 115200;
}

void main(void)
{
    float value;
    
    i2c_struct_init();
    uart_struct_init();
    i2c_init(&i2c);
    uart_init(&uart);

    printf("hello world!\r\n");
    
    if(LM75A_ADDR == i2c.own_addr){
        while(1){
            wBuf[0]=0x00;
            dwc_i2c_write(&i2c, wBuf, 1);
            dwc_i2c_read(&i2c, rBuf, 2);
            value = ((rBuf[0]&0x7F)<<3)|(rBuf[1]>>5);
            printf("%4.2f¡æ\r\n", value/8);
            udelay(100000);
        }
    }
    else if(AT24C32_ADDR == i2c.own_addr){
        wBuf[0]=0x00;
        wBuf[1]=0xA1;
        wBuf[2]=0xA2;
        wBuf[3]=0xA3;
        wBuf[4]=0xA4;
        wBuf[5]=0xA5;
        dwc_i2c_write(&i2c, wBuf, 6);
        dwc_i2c_write(&i2c, wBuf, 1);
        dwc_i2c_read(&i2c,rBuf,4);
        rBuf[5] = rBuf[5];
        while(1);
    }
    else{
    
    }
}