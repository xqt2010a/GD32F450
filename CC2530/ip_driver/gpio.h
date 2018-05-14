#ifndef __GPIO_H__
#define __GPIO_H__

#include <ioCC2530.h>

#define LED1 P2_0
#define LED2 P2_1

#define SPI_CS   P1_0
#define SPI_SCLK P1_1
#define SPI_MISO P1_2
#define SPI_MOSI P1_3
#define SPI_RST  P1_7
#define SPI_INT  P1_5

extern void GPIO_Init(void);

extern void LED1_On(void);
extern void LED2_On(void);
extern void LED1_Off(void);
extern void LED2_Off(void);

#endif  /* __GPIO_H__ */