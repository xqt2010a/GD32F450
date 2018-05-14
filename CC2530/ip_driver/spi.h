#ifndef __SPI_H__
#define __SPI_H__

#include <ioCC2530.h>
#include "type.h"

//#define SPI_HARDWARE  1  Ó²¼þSPI

#define SPI_RST    P1_3
#define SPI_CS     P1_4
#define SPI_CLK    P1_5
#define SPI_MOSI   P1_6
#define SPI_MISO   P1_7

extern void SPI_Init(void);
extern unsigned char SPI_ReadByte(void);
extern void SPI_WriteByte(unsigned char data);

extern uint8_t spi_SetReg(uint8_t ucRegAddr, uint8_t ucRegVal);
extern uint8_t spi_GetReg(uint8_t ucRegAddr);
extern uint8_t spi_GetReg2(uint8_t ucRegAddr, uint8_t *p);

#endif  /* __SPI_H__ */