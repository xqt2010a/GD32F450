#include "rf.h"
#include "stdio.h"
#include "uart.h"

void Rf_Init(void)
{
    TXPOWER   = 0xD5;                 // 发射功率为1dBm
    CCACTRL0  = 0xF8;                 // 推荐值 smartRF软件生成

    FRMFILT0  = 0x0C;                 // 静止接收过滤，即接收所有数据包

    FSCAL1 =    0x00;                 // 推荐值 smartRF软件生成
    TXFILTCFG = 0x09;
    AGCCTRL1 =  0x15;
    AGCCTRL2 =  0xFE;

    TXFILTCFG = 0x09;                 // 推荐值 smartRF软件生成

    FREQCTRL = 0x0B;                  // 选择通道11

    RFIRQM0 |= (1<<6);                // 使能RF数据包接收中断
    IEN2 |= (1<<0);                   // 使能RF中断

    RFST = 0xED;                      // 清除RF接收缓冲区 ISFLUSHRX
    RFST = 0xE3;                      // RF接收使能 ISRXON
}

void Rf_Send( unsigned char *pbuf , unsigned int len)
{
    RFST = 0xE3;                      // RF接收使能 ISRXON
    // 等待发送状态不活跃 并且 没有接收到SFD
    while( FSMSTAT1 & (( 1<<1 ) | ( 1<<5 )));

    RFIRQM0 &= ~(1<<6);               // 禁止接收数据包中断
    IEN2 &= ~(1<<0);                  // 清除RF全局中断

    RFST = 0xEE;                      // 清除发送缓冲区 ISFLUSHTX
    RFIRQF1 = ~(1<<1);                // 清除发送完成标志

    // 填充缓冲区 填充过程需要增加2字节，CRC校验自动填充
    RFD = len + 2;
    for (int i = 0; i < len; i++)
    {
        RFD = *pbuf++;
    }

    RFST = 0xE9;                      // 发送数据包 ISTXON
    while (!(RFIRQF1 & (1<<1)));      // 等待发送完成
    RFIRQF1 = ~(1<<1);                // 清除发送完成标志位

    RFIRQM0 |= (1<<6);                // RX接收中断
    IEN2 |= (1<<0);
}

unsigned int Rf_Receive_Isr(unsigned char * rf_rx_buf)
{
    unsigned int rf_rx_len = 0;
    int rssi = 0;
    char crc_ok = 0;

    rf_rx_len = RFD - 2;                        // 长度去除两字节附加结果
    rf_rx_len &= 0x7F;
    for (int i = 0; i < rf_rx_len; i++)
    {
        rf_rx_buf[i] = RFD;                       // 连续读取接收缓冲区内容
    }

    rssi = RFD - 73;                            // 读取RSSI结果
    crc_ok = RFD;                               // 读取CRC校验结果 BIT7

    RFST = 0xED;                                // 清除接收缓冲区
    if( crc_ok & 0x80 )
    {
        //Uart0_Sendbuf( rf_rx_buf , rf_rx_len);    // 串口发送
        printf("[%d]\r\n",rssi);
    }
    else
    {
        printf("\r\nCRC Error\r\n");
    }
	return rf_rx_len;
}