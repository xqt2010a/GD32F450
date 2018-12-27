#ifndef __JX_H__
#define __JX_H__

#include "stdint.h"

#define SYS_CPU_BASE        0xF00000
#define SYS_MCU_BASE        0xF04000
#define SYS_WDT_BASE        0x1908000
#define SYS_INT_BASE        0x190C000
#define SYS_DMA_BASE        0x190E000
#define SYS_ADC_BASE        0x3FE00000
#define SYS_I2C_BASE        0x3FF00000
#define SYS_SPI_BASE        0x3FF20000
#define SYS_UART_BASE       0x3FF24000
#define SYS_I2S_BASE        0x3FF28000



#define SYS_GIC_BASE        SYS_CPU_BASE    //SYS_MCU_BASE

#define JX_WR4(x)    (*(volatile unsigned int *)(x))

void register_write(uint32_t addr, uint8_t mask_bit, uint8_t start_bit, uint32_t value);
uint32_t register_read(uint32_t addr, uint8_t mask_bit, uint8_t start_bit);

#endif  /* __JX_H__ */