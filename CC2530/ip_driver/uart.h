#ifndef __UART_H__
#define __UART_H__

extern unsigned char Uart_RxData[128];
extern unsigned char Uart_Rxi;
extern unsigned char rx_flag;

extern void Uart0_Init(void);
extern void Uart0_Sendbuf(unsigned char *pbuf , unsigned int len);

#endif  /* __UART_H__ */