#ifndef __DWC_I2S_H__
#define __DWC_I2S_H__

#include "config.h"

#define I2S_BASE(x)     (SYS_I2S_BASE+0x1000*(x))

#define I2S_IER(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x000))   //DW_apb_i2s Enable Register
#define I2S_IRER(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x004))   //I2S Receiver Block Enable Register
#define I2S_ITER(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x008))   //I2S Transmitter Block Enable Register
#define I2S_CER(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x00C))   //Clock Enable Register
#define I2S_CCR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x010))   //Clock Configuration Register
#define I2S_RXFFR(ch)        (*(volatile unsigned int *)(I2S_BASE(ch) + 0x014))   //Receiver Block FIFO Register   
#define I2S_TXFFR(ch)        (*(volatile unsigned int *)(I2S_BASE(ch) + 0x018))   //Transmitter Block FIFO Register

#define I2S_LRBR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x020))   //Left Receive Buffer 0
#define I2S_LTHR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x020))   //Left Transmit Holding Register 0
#define I2S_RRBR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x024))   //Right Receive Buffer 0
#define I2S_RTHR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x024))   //Right Transmit Holding Register 0
#define I2S_RER(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x028))   //Receive Enable Register 0
#define I2S_TER(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x02C))   //Transmit Enable Register 0
#define I2S_RCR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x030))   //Receive Configuration Register 0
#define I2S_TCR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x034))   //Transmit Configuration Register 0
#define I2S_ISR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x038))   //Interrupt Status Register 0
#define I2S_IMR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x03C))   //Interrupt Mask Register 0
#define I2S_ROR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x040))   //Receive Overrun Register 0
#define I2S_TOR(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x044))   //Transmit Overrun Register 0
#define I2S_RFCR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x048))   //Receive FIFO Configuration Register 0
#define I2S_TFCR(ch)         (*(volatile unsigned int *)(I2S_BASE(ch) + 0x04C))   //Transmit FIFO Configuration Register 0
#define I2S_RFF(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x050))   //Receive FIFO Flush 0
#define I2S_TFF(ch)          (*(volatile unsigned int *)(I2S_BASE(ch) + 0x054))   //Transmit FIFO Flush 0

#define I2S_RXDMA(ch)        (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1C0))   //Receiver Block DMA Register
#define I2S_RRXDMA(ch)       (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1C4))   //Reset Receiver Block DMA Register
#define I2S_TXDMA(ch)        (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1C8))   //Transmitter Block DMA Register
#define I2S_RTXDMA(ch)       (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1CC))   //Reset Transmitter Block DMA Register
#define I2S_COMP_PARAM_2(ch) (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1F0))   //Component Parameter 2 Register
#define I2S_COMP_PARAM_1(ch) (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1F4))   //Component Parameter 1 Register
#define I2S_COMP_VERSION(ch) (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1F8))   //Component Version ID
#define I2S_COMP_TYPE(ch)    (*(volatile unsigned int *)(I2S_BASE(ch) + 0x1FC))   //DesignWare Component Type

/**
  * @brief  I2S Init structure definition
  */
  
typedef struct
{
    uint8_t ch;                         /* ch = 0, 1, 2*/
    uint8_t bit_rlt;                 /*!< bit resolution 1:12b, 2:16b, 3:20b, 4:24b, 5:32b */
    uint32_t sclk;                  /*!< sample_fs * 2 * bit resolution*/
    uint32_t mclk;                  /*!< sample_fs * 256 */
    uint8_t clk_cyc;                 /*!< Clock cycles 0:16, 1:24, 2:32, */
    uint16_t int_mask;		/*!< See in datesheet */
} I2S_InitTypeDef;

void dwc_init(I2S_InitTypeDef * i2s);
void dwc_i2s_write(I2S_InitTypeDef * i2s, uint32_t LValue, uint32_t RValue);

#endif  /* __DWC_I2S_H__ */