#include "rf.h"
#include "stdio.h"
#include "uart.h"

void Rf_Init(void)
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

void Rf_Send( unsigned char *pbuf , unsigned int len)
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

unsigned int Rf_Receive_Isr(unsigned char * rf_rx_buf)
{
    unsigned int rf_rx_len = 0;
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
        //Uart0_Sendbuf( rf_rx_buf , rf_rx_len);    // ���ڷ���
        printf("[%d]\r\n",rssi);
    }
    else
    {
        printf("\r\nCRC Error\r\n");
    }
	return rf_rx_len;
}