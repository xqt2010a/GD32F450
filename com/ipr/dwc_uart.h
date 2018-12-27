#ifndef __DWC_UART_H__
#define __DWC_UART_H__

#include "config.h"

#define UART_BASE(x)     (SYS_UART_BASE+0x1000*(x))

#define UART_RBR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x00))	// the receive buffer register
#define UART_THR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x00))	// the transmit holding register
#define UART_DLL(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x00))	// the divisor latch( low )
#define UART_DLH(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x04))	// the divisor latch( high )
#define UART_IER(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x04))	// the interupt enable register
#define UART_IIR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x08))	// the interupt identification register
#define UART_FCR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x08))	// the fifo control register
#define UART_LCR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x0C))	// the line control register
#define UART_MCR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x10))	// the modem control register
#define UART_LSR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x14))	// the line status register
#define UART_MSR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x18))	// the modem status register
#define UART_SCR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x1C))	// the scratchpad register
#define UART_LPDLL(ch) (*(volatile unsigned int *)(UART_BASE(ch) + 0x20))	// the low power divisor latch( low ) register
#define UART_LPDLH(ch) (*(volatile unsigned int *)(UART_BASE(ch) + 0x24))	// the low power divisor latch( high ) register
/* Reserved 0x28 --> 0x2C */
#define UART_SRBR(ch)  (*(volatile unsigned int *)(UART_BASE(ch) + 0x30 /* --> 0x6C */ ))// the shadow receive buffer register
#define UART_STHR(ch)  (*(volatile unsigned int *)(UART_BASE(ch) + 0x30 /* --> 0x6C */ ))// the shadow transmit holding register
#define UART_FAR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x70))	// the fifo access register
#define UART_TFR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x74))	// the transmit fifo read
#define UART_RFW(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x78))	// the receive fifo write
#define UART_USR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x7C))	// the uart status register
#define UART_TFL(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x80))	// the transmit fifo level
#define UART_RFL(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x84))	// the receive fifo level
#define UART_SRR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x88))	// the software reset register
#define UART_SRTS(ch)  (*(volatile unsigned int *)(UART_BASE(ch) + 0x8C))	// the shadow request to send
#define UART_SBCR(ch)  (*(volatile unsigned int *)(UART_BASE(ch) + 0x90))	// the shadow break control register
#define UART_SDMAM(ch) (*(volatile unsigned int *)(UART_BASE(ch) + 0x94))	// the shadow dma mode
#define UART_SFE(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x98))	// the shadow fifo enable
#define UART_SRT(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0x9C))	// the shadow rcvr register
#define UART_STET(ch)  (*(volatile unsigned int *)(UART_BASE(ch) + 0xA0))	// the shadow tx empty trigger
#define UART_HTX(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0xA4))	// the halt tx
#define UART_DMASA(ch) (*(volatile unsigned int *)(UART_BASE(ch) + 0xA8))	// the dma software acknowledge
#define UART_CPR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0xF4))	// the component parameter register
#define UART_UCV(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0xF8))	// the uart component version
#define UART_CTR(ch)   (*(volatile unsigned int *)(UART_BASE(ch) + 0xFC))	// the component type register

/**
  * @brief  UART Init structure definition
  */
  
typedef struct
{
    uint8_t ch;
    uint8_t int_mask;    /* see datasheet */
    uint32_t clk;
    uint32_t bandrate;
} UART_InitTypeDef;

void dwc_uart_init(UART_InitTypeDef *uart);
void dwc_uart_tx(UART_InitTypeDef *uart, uint16_t data);
uint8_t dwc_uart_rx(UART_InitTypeDef *uart);

#endif  /* __DWC_UART_H__ */