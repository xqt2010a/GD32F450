#ifndef __DWC_SPI_H__
#define __DWC_SPI_H__

#define SPI_BASE(x)     (SYS_SPI_BASE+0x1000*(x))

#define SPI_CTRLR0(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x00))   	//Control Register 0
#define SPI_CTRLR1(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x04))   	//Control Register 1
#define SPI_SSIENR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x08))   	//SSI Enable Register
#define SPI_MWCR(ch)          (*(volatile unsigned int *)(SPI_BASE(ch) + 0x0C))   	//Microwire Control Register
#define SPI_SER(ch)           (*(volatile unsigned int *)(SPI_BASE(ch) + 0x10))   	//Slave Enable Register
#define SPI_BAUDR(ch)         (*(volatile unsigned int *)(SPI_BASE(ch) + 0x14))   	//Baud Rate Select
#define SPI_TXFTLR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x18))   	//Transmit FIFO Threshold Level
#define SPI_RXFTLR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x1C))   	//Receive FIFO Threshold Level
#define SPI_TXFLR(ch)         (*(volatile unsigned int *)(SPI_BASE(ch) + 0x20))   	//Transmit FIFO Level Register
#define SPI_RXFLR(ch)         (*(volatile unsigned int *)(SPI_BASE(ch) + 0x24))   	//Receive FIFO Level Register
#define SPI_SR(ch)            (*(volatile unsigned int *)(SPI_BASE(ch) + 0x28))   	//Status Register
#define SPI_IMR(ch)           (*(volatile unsigned int *)(SPI_BASE(ch) + 0x2C))   	//Interrupt Mask Register
#define SPI_ISR(ch)           (*(volatile unsigned int *)(SPI_BASE(ch) + 0x30))   	//Interrupt Status Register
#define SPI_RISR(ch)          (*(volatile unsigned int *)(SPI_BASE(ch) + 0x34))   	//Raw Interrupt Status Register
#define SPI_TXOICR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x38))   	//Transmit FIFO Overflow Interrupt Clear Register
#define SPI_RXOICR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x3C))   	//Receive FIFO Overflow Interrupt Clear Register
#define SPI_RXUICR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x40))   	//Receive FIFO Underflow Interrupt Clear Register
#define SPI_MSTICR(ch)        (*(volatile unsigned int *)(SPI_BASE(ch) + 0x44))   	//Multi-Master Interrupt Clear Register
#define SPI_ICR(ch)           (*(volatile unsigned int *)(SPI_BASE(ch) + 0x48))   	//Interrupt Clear Register
#define SPI_DMACR(ch)         (*(volatile unsigned int *)(SPI_BASE(ch) + 0x4C))   	//DMA Control Register
#define SPI_DMATDLR(ch)       (*(volatile unsigned int *)(SPI_BASE(ch) + 0x50))   	//DMA Transmit Data Level
#define SPI_DMARDLR(ch)       (*(volatile unsigned int *)(SPI_BASE(ch) + 0x54))   	//DMA Receive Data Level
#define SPI_IDR(ch)           (*(volatile unsigned int *)(SPI_BASE(ch) + 0x58))   	//Identification Register
#define SPI_COMP_VERSION(ch)  (*(volatile unsigned int *)(SPI_BASE(ch) + 0x5C))   	//coreKit version ID register
#define SPI_DR(ch)            (*(volatile unsigned int *)(SPI_BASE(ch) + 0x60))   	//Data Register
#define SPI_RX_SAMPLE_DLY(ch) (*(volatile unsigned int *)(SPI_BASE(ch) + 0xf0))   	//RXD Sample Delay Register

#endif  /* __DWC_SPI_H__ */