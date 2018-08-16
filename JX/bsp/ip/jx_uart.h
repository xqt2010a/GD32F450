#ifndef __JX_UART_H__
#define __JX_UART_H__

typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#define UART_CLK 22000000  //22M

#define UART0_BASE         0x3ff24000
#define UART1_BASE         0x3ff25000
#define UART2_BASE         0x3ff26000
#define UART3_BASE         0x3ff27000

/* ------------------------  UARTn REG DEF ---------------------------------  */    // n(0,1,2)
#define UART_RBR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x00))	// the receive buffer register
#define UART_THR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x00))	// the transmit holding register
#define UART_DLL(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x00))	// the divisor latch( low )
#define UART_DLH(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x04))	// the divisor latch( high )
#define UART_IER(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x04))	// the interupt enable register
#define UART_IIR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x08))	// the interupt identification register
#define UART_FCR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x08))	// the fifo control register
#define UART_LCR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x0C))	// the line control register
#define UART_MCR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x10))	// the modem control register
#define UART_LSR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x14))	// the line status register
#define UART_MSR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x18))	// the modem status register
#define UART_SCR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x1C))	// the scratchpad register
#define UART_LPDLL(UARTn_BASE) (*(volatile unsigned long*)((UARTn_BASE) + 0x20))	// the low power divisor latch( low ) register
#define UART_LPDLH(UARTn_BASE) (*(volatile unsigned long*)((UARTn_BASE) + 0x24))	// the low power divisor latch( high ) register
	/* Reserved 0x28 --> 0x2C */
#define UART_SRBR(UARTn_BASE)  (*(volatile unsigned long*)((UARTn_BASE) + 0x30 /* --> 0x6C */ ))// the shadow receive buffer register
#define UART_STHR(UARTn_BASE)  (*(volatile unsigned long*)((UARTn_BASE) + 0x30 /* --> 0x6C */ ))// the shadow transmit holding register
#define UART_FAR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x70))	// the fifo access register
#define UART_TFR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x74))	// the transmit fifo read
#define UART_RFW(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x78))	// the receive fifo write
#define UART_USR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x7C))	// the uart status register
#define UART_TFL(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x80))	// the transmit fifo level
#define UART_RFL(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x84))	// the receive fifo level
#define UART_SRR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x88))	// the software reset register
#define UART_SRTS(UARTn_BASE)  (*(volatile unsigned long*)((UARTn_BASE) + 0x8C))	// the shadow request to send
#define UART_SBCR(UARTn_BASE)  (*(volatile unsigned long*)((UARTn_BASE) + 0x90))	// the shadow break control register
#define UART_SDMAM(UARTn_BASE) (*(volatile unsigned long*)((UARTn_BASE) + 0x94))	// the shadow dma mode
#define UART_SFE(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x98))	// the shadow fifo enable
#define UART_SRT(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0x9C))	// the shadow rcvr register
#define UART_STET(UARTn_BASE)  (*(volatile unsigned long*)((UARTn_BASE) + 0xA0))	// the shadow tx empty trigger
#define UART_HTX(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0xA4))	// the halt tx
#define UART_DMASA(UARTn_BASE) (*(volatile unsigned long*)((UARTn_BASE) + 0xA8))	// the dma software acknowledge
#define UART_CPR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0xF4))	// the component parameter register
#define UART_UCV(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0xF8))	// the uart component version
#define UART_CTR(UARTn_BASE)   (*(volatile unsigned long*)((UARTn_BASE) + 0xFC))	// the component type register

void uart_init(unsigned int bd);
void uart_tx(uint16_t Data);
uint16_t uart_rx(void);


#endif /* __JX_UART_H__ */