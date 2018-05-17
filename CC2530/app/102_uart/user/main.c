#include "ioCC2530.h"
#include <stdio.h>
#include <string.h>

#define LED1 P1_0
#define LED2 P1_1
char rf_rx_buf[128];

char serial_rxbuf[128];       // ���ڽ��ջ�����
int  serial_rxpos = 0;
int  serial_rxlen = 0;
char is_serial_receive = 0;

int Rssi_Vale = 0;

void uart0_init();
void uart0_sendbuf(char *pbuf , int len);
void uart0_flush_rxbuf();

void timer1_init();
void timer1_disbale();
void timer1_enable();

void rf_send( char *pbuf , int len);
void rf_receive_isr();

void uart0_init()
{
    PERCFG = 0x00;              // UART0 ѡ��λ��0 TX@P0.3 RX@P0.2
    P0SEL |= 0x0C;              // P0.3 P0.2ѡ�����蹦��
    U0CSR |= 0xC0;              // UARTģʽ ������ʹ��
    U0GCR |= 11;                // ����� U0GCR �� U0BAUD
    U0BAUD = 216;               // 115200
    UTX0IF = 0;

    URX0IE = 1;                 // ʹ�ܽ����ж� IEN0@BIT2
}

//void uart0_init()
//{
//    PERCFG = 0x00;   // ������ƣ�UART0ѡ��˿�λ��1 
//    P0SEL = 0x0C;    // �˿�0����ѡ��P0_2��P0_3��������
//    P2DIR &= ~0xC0;  // �˿�0�������ȼ����ƣ���2λ��0��USART0����
//
//    U0CSR |= 0x80;   // UARTģʽ
//    U0GCR |= 11;
//    U0BAUD |= 216;   // ����32MHzϵͳʱ��ʱ������������Ϊ115200
//    UTX0IF = 0;      // UART0 TX�жϱ�־��ʼ��λ0
//    
//}

void uart0_flush_rxbuf()
{
    serial_rxpos = 0;
    serial_rxlen = 0;
}

void timer1_init()
{
    T1CTL = 0x0C;         // @DIV��Ƶϵ�� 128 @MODE��ͣ����
    T1CCTL0 = 0x44;       // @IMͨ��0�ж�ʹ�� @MODE�Ƚ�ƥ��ģʽ
    T1STAT = 0x00;        // ��������жϱ�־

    T1IE = 1;             // IEN1@BIT1 ʹ�ܶ�ʱ��1�ж�

    T1CC0L = 250;         // �������Ϊ2ms
    T1CC0H = 0;
}

void timer1_disbale()
{
    T1CTL &= ~( 1<< 1);   // �ָ�Ϊֹͣģʽ
}

void timer1_enable()
{
    T1CTL |= ( 1 << 1 );  // �ı�ģʽΪ�Ƚ�ƥ��ģʽ MODE = 0x10;
    T1STAT = 0x00;        // ����жϱ�־λ
    T1CNTH = 0;           // ���¿�ʼ����
    T1CNTL = 0;
}

void rf_init()
{
    TXPOWER   = 0xD5;                 // ���书��Ϊ1dBm
    CCACTRL0  = 0xF8;                 // �Ƽ�ֵ smartRF�������

    FRMFILT0  = 0x0C;                 // ��ֹ���չ��ˣ��������������ݰ�

    FSCAL1 =    0x00;                 // �Ƽ�ֵ smartRF�������
    TXFILTCFG = 0x09;
    AGCCTRL1 =  0x15;
    AGCCTRL2 =  0xFE;

    TXFILTCFG = 0x09;                 // �Ƽ�ֵ smartRF�������

    FREQCTRL = 0x0B;                  // ѡ��ͨ��11

    RFIRQM0 |= (1<<6);                // ʹ��RF���ݰ������ж�
    IEN2 |= (1<<0);                   // ʹ��RF�ж�

    RFST = 0xED;                      // ���RF���ջ����� ISFLUSHRX
    RFST = 0xE3;                      // RF����ʹ�� ISRXON
}

void rf_send( char *pbuf , int len)
{
    RFST = 0xE3;                      // RF����ʹ�� ISRXON
    // �ȴ�����״̬����Ծ ���� û�н��յ�SFD
    while( FSMSTAT1 & (( 1<<1 ) | ( 1<<5 )));

    RFIRQM0 &= ~(1<<6);               // ��ֹ�������ݰ��ж�
    IEN2 &= ~(1<<0);                  // ���RFȫ���ж�

    RFST = 0xEE;                      // ������ͻ����� ISFLUSHTX
    RFIRQF1 = ~(1<<1);                // ���������ɱ�־

    // ��仺���� ��������Ҫ����2�ֽڣ�CRCУ���Զ����
    RFD = len + 2;
    for (int i = 0; i < len; i++)
    {
        RFD = *pbuf++;
    }

    RFST = 0xE9;                      // �������ݰ� ISTXON
    while (!(RFIRQF1 & (1<<1)));      // �ȴ��������
    RFIRQF1 = ~(1<<1);                // ���������ɱ�־λ

    RFIRQM0 |= (1<<6);                // RX�����ж�
    IEN2 |= (1<<0);
}

void rf_receive_isr()
{
    int rf_rx_len = 0;
    int rssi = 0;
    char crc_ok = 0;

    rf_rx_len = RFD - 2;                        // ����ȥ�����ֽڸ��ӽ��
    rf_rx_len &= 0x7F;
    for (int i = 0; i < rf_rx_len; i++)
    {
        rf_rx_buf[i] = RFD;                       // ������ȡ���ջ���������
    }

    rssi = RFD - 73;                            // ��ȡRSSI���
    crc_ok = RFD;                               // ��ȡCRCУ���� BIT7

    RFST = 0xED;                                // ������ջ�����
    if( crc_ok & 0x80 )
    {
        uart0_sendbuf( rf_rx_buf , rf_rx_len);    // ���ڷ���
        //printf("[%d]\r\n",rssi);
    }
    else
    {
        //printf("\r\nCRC Error\r\n");
    }
    Rssi_Vale = rssi;
}

void main(void)
{
    P1DIR |= ( 1<< 0) | (1<<1 );                // P1.0 ���
    LED1 = 0; LED2 = 0;

    EA = 0;                                     // ��ʱ�ر�ȫ���ж�

    SLEEPCMD &= ~0x04;                          // ����ϵͳʱ��Ϊ32MHz
    while( !(SLEEPSTA & 0x40) );
    CLKCONCMD &= ~0x47;
    SLEEPCMD |= 0x04;

    uart0_init();                               // ���ڳ�ʼ�� 115200
    timer1_init();                              // ��ʱ����ʼ�� 2ms �Ƚ�ƥ��
    rf_init();                                  // RF��ʼ�� ��֡����

    EA = 1;                                     // ʹ��ȫ���ж�

    //printf("LIP Master\r\n");
    //printf("Code by Labc!\r\n");

    while(1)
    {
        if( is_serial_receive )                   // ���յ��������ݰ�
        {
            is_serial_receive = 0;                  // �����־λ
            EA = 0;
            rf_send(serial_rxbuf , serial_rxlen);   // ֱ��ת����������
            EA = 1;
            uart0_flush_rxbuf();                    // ������ڽ��ջ�����
        }
    }
}

int putchar(int c)
{
    U0DBUF = c;
    while( !UTX0IF );
    UTX0IF = 0;
    return c;
}

void uart0_sendbuf(char *pbuf , int len)
{
    for( int i = 0 ; i < len ; i++)
    {
        U0DBUF = *pbuf++;
        while(!UTX0IF);
        UTX0IF = 0;
    }
}

#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{
    URX0IF = 0;                                   // ��������жϱ�־
    serial_rxbuf[serial_rxpos] = U0DBUF;          // ��仺����
    serial_rxpos++;
    serial_rxlen++;

    timer1_enable();                              // ��ʱ�����¿�ʼ����
}


#pragma vector=T1_VECTOR
__interrupt void Timer1_ISR(void)
{
    T1STAT &= ~( 1<< 0);                          // �����ʱ��T1ͨ��0�жϱ�־

    is_serial_receive = 1;                        // �������ݵ���
    timer1_disbale();
}

#pragma vector=RF_VECTOR
__interrupt void rf_isr(void)
{
    LED1 ^= 1;                                    // LED1��ת ��ʾ����
    EA = 0;

    // ���յ�һ�����������ݰ�
    if (RFIRQF0 & ( 1<<6 ))
    {
        rf_receive_isr();                           // ���ý����жϴ�����

        S1CON = 0;                                  // ���RF�жϱ�־
        RFIRQF0 &= ~(1<<6);                         // ���RF����������ݰ��ж�
    }
    EA = 1;
}
