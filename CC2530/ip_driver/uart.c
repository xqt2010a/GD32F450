#include "ioCC2530.h"
#include <stdio.h>
#include "uart.h"

unsigned char Uart_RxData[128];
unsigned char Uart_Rxi;
unsigned char rx_flag;


void Uart0_Init(void)
{
    PERCFG = 0x00;              // UART0 ѡ��λ��0 TX@P0.3 RX@P0.2
    P0SEL |= 0x0C;              // P0.3 P0.2ѡ�����蹦��
    U0CSR |= 0xC0;              // UARTģʽ ������ʹ��
    U0GCR |= 11;                // ����� U0GCR �� U0BAUD
    U0BAUD = 216;               // 115200
    UTX0IF = 1;

    URX0IE = 1;                 // ʹ�ܽ����ж� IEN0@BIT2
}



int putchar(int c)
{
    while( !UTX0IF );
    UTX0IF = 0;
    U0DBUF = c;
    return c;
}

void Uart0_Sendbuf(unsigned char *pbuf , unsigned int len)
{
    for( int i = 0 ; i < len ; i++)
    {
        while(!UTX0IF);
        UTX0IF = 0;
        U0DBUF = *pbuf;
        pbuf++;
    }
}



#pragma vector=URX0_VECTOR  
__interrupt void UART0_ISR(void)  
{    
    URX0IF = 0;            // ��������жϱ�־  
    Uart_RxData[Uart_Rxi] = U0DBUF;
    Uart_Rxi++;
}