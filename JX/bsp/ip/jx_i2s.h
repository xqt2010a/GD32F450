#ifndef __JX_I2S_H__
#define __JX_I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
    
#define I2S_BASE0       0x3ff28000
#define I2S_BASE1       0x3ff29000
#define I2S_BASE2       0x3ff2a000
#define I2S_BASE        I2S_BASE0

/* ------------------------  I2S REG DEF -----------------------------------  */
#define I2S_IER    			   (*(volatile unsigned long*)(I2S_BASE + 0x000))   //DW_apb_i2s Enable Register
#define I2S_IRER    		   (*(volatile unsigned long*)(I2S_BASE + 0x004))   //I2S Receiver Block Enable Register
#define I2S_ITER    		   (*(volatile unsigned long*)(I2S_BASE + 0x008))   //I2S Transmitter Block Enable Register
#define I2S_CER    			   (*(volatile unsigned long*)(I2S_BASE + 0x00C))   //Clock Enable Register
#define I2S_CCR    			   (*(volatile unsigned long*)(I2S_BASE + 0x010))   //Clock Configuration Register
#define I2S_RXFFR    		   (*(volatile unsigned long*)(I2S_BASE + 0x014))   //Receiver Block FIFO Register
#define I2S_TXFFR    		   (*(volatile unsigned long*)(I2S_BASE + 0x018))   //Transmitter Block FIFO Register
#define I2S_LRBR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x020))   //Left Receive Buffer 0
#define I2S_LTHR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x020))   //Left Transmit Holding Register 0
#define I2S_RRBR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x024))   //Right Receive Buffer 0
#define I2S_RTHR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x024))   //Right Transmit Holding Register 0
#define I2S_RER0    		   (*(volatile unsigned long*)(I2S_BASE + 0x028))   //Receive Enable Register 0
#define I2S_TER0    		   (*(volatile unsigned long*)(I2S_BASE + 0x02C))   //Transmit Enable Register 0
#define I2S_RCR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x030))   //Receive Configuration Register 0
#define I2S_TCR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x034))   //Transmit Configuration Register 0
#define I2S_ISR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x038))   //Interrupt Status Register 0
#define I2S_IMR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x03C))   //Interrupt Mask Register 0
#define I2S_ROR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x040))   //Receive Overrun Register 0
#define I2S_TOR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x044))   //Transmit Overrun Register 0
#define I2S_RFCR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x048))   //Receive FIFO Configuration Register 0
#define I2S_TFCR0    		   (*(volatile unsigned long*)(I2S_BASE + 0x04C))   //Transmit FIFO Configuration Register 0
#define I2S_RFF0    		   (*(volatile unsigned long*)(I2S_BASE + 0x050))   //Receive FIFO Flush 0
#define I2S_TFF0    		   (*(volatile unsigned long*)(I2S_BASE + 0x054))   //Transmit FIFO Flush 0
#define I2S_LRBR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x060))   //Left Receive Buffer 1
#define I2S_LTHR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x060))   //Left Transmit Holding Register 1
#define I2S_RRBR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x064))   //Right Receive Buffer 1
#define I2S_RTHR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x064))   //Right Transmit Holding Register 1
#define I2S_RER1    		   (*(volatile unsigned long*)(I2S_BASE + 0x068))   //Receive Enable Register 1
#define I2S_TER1    		   (*(volatile unsigned long*)(I2S_BASE + 0x06C))   //Transmit Enable Register 1
#define I2S_RCR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x070))   //Receive Configuration Register 1
#define I2S_TCR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x074))   //Transmit Configuration Register 1
#define I2S_ISR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x078))   //Interrupt Status Register 1
#define I2S_IMR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x07C))   //Interrupt Mask Register 1
#define I2S_ROR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x080))   //Receive Overrun Register 1
#define I2S_TOR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x084))   //Transmit Overrun Register 1
#define I2S_RFCR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x088))   //Receive FIFO Configuration Register 1
#define I2S_TFCR1    		   (*(volatile unsigned long*)(I2S_BASE + 0x08C))   //Transmit FIFO Configuration Register 1
#define I2S_RFF1    		   (*(volatile unsigned long*)(I2S_BASE + 0x090))   //Receive FIFO Flush 1
#define I2S_TFF1    		   (*(volatile unsigned long*)(I2S_BASE + 0x094))   //Transmit FIFO Flush 1
#define I2S_LRBR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0A0))   //Left Receive Buffer 0
#define I2S_LTHR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0A0))   //Left Transmit Holding Register 2
#define I2S_RRBR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0A4))   //Right Receive Buffer 2
#define I2S_RTHR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0A4))   //Right Transmit Holding Register 2
#define I2S_RER2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0A8))   //Receive Enable Register 2
#define I2S_TER2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0AC))   //Transmit Enable Register 2
#define I2S_RCR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0B0))   //Receive Configuration Register 2
#define I2S_TCR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0B4))   //Transmit Configuration Register 2
#define I2S_ISR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0B8))   //Interrupt Status Register 2
#define I2S_IMR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0BC))   //Interrupt Mask Register 2
#define I2S_ROR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0C0))   //Receive Overrun Register 2
#define I2S_TOR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0C4))   //Transmit Overrun Register 2
#define I2S_RFCR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0C8))   //Receive FIFO Configuration Register 2
#define I2S_TFCR2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0CC))   //Transmit FIFO Configuration Register 2
#define I2S_RFF2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0D0))   //Receive FIFO Flush 2
#define I2S_TFF2    		   (*(volatile unsigned long*)(I2S_BASE + 0x0D4))   //Transmit FIFO Flush 2
#define I2S_LRBR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0E0))   //Left Receive Buffer 3
#define I2S_LTHR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0E0))   //Left Transmit Holding Register 3
#define I2S_RRBR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0E4))   //Right Receive Buffer 3
#define I2S_RTHR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0E4))   //Right Transmit Holding Register 3
#define I2S_RER3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0E8))   //Receive Enable Register 3
#define I2S_TER3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0EC))   //Transmit Enable Register 3
#define I2S_RCR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0F0))   //Receive Configuration Register 3
#define I2S_TCR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0F4))   //Transmit Configuration Register 3
#define I2S_ISR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0F8))   //Interrupt Status Register 3
#define I2S_IMR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x0FC))   //Interrupt Mask Register 3
#define I2S_ROR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x100))   //Receive Overrun Register 3
#define I2S_TOR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x104))   //Transmit Overrun Register 3
#define I2S_RFCR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x108))   //Receive FIFO Configuration Register 3
#define I2S_TFCR3    		   (*(volatile unsigned long*)(I2S_BASE + 0x10C))   //Transmit FIFO Configuration Register 3
#define I2S_RFF3    		   (*(volatile unsigned long*)(I2S_BASE + 0x110))   //Receive FIFO Flush 3
#define I2S_TFF3    		   (*(volatile unsigned long*)(I2S_BASE + 0x114))   //Transmit FIFO Flush 3
#define I2S_RXDMA    		   (*(volatile unsigned long*)(I2S_BASE + 0x1C0))   //Receiver Block DMA Register
#define I2S_RRXDMA    		   (*(volatile unsigned long*)(I2S_BASE + 0x1C4))   //Reset Receiver Block DMA Register
#define I2S_TXDMA    		   (*(volatile unsigned long*)(I2S_BASE + 0x1C8))   //Transmitter Block DMA Register
#define I2S_RTXDMA    		   (*(volatile unsigned long*)(I2S_BASE + 0x1CC))   //Reset Transmitter Block DMA Register
#define I2S_COMP_PARAM_2       (*(volatile unsigned long*)(I2S_BASE + 0x1F0))   //Component Parameter 2 Register
#define I2S_COMP_PARAM_1       (*(volatile unsigned long*)(I2S_BASE + 0x1F4))   //Component Parameter 1 Register
#define I2S_COMP_VERSION       (*(volatile unsigned long*)(I2S_BASE + 0x1F8))   //Component Version ID
#define I2S_COMP_TYPE    	   (*(volatile unsigned long*)(I2S_BASE + 0x1FC))   //DesignWare Component Type
    
/**
  * @brief  I2S Init structure definition
  */

typedef struct
{
    uint8_t I2S_BitRlt;            /*!< bit resolution 1:12b, 2:16b, 3:20b, 4:24b, 5:32b */
	
	uint8_t I2S_SclkDiv;           /*!< Max 14b */
	
	uint8_t I2S_MclkDiv;           /*!< Max 10b */

	uint8_t I2S_ClkCyc;            /*!< Clock cycles 0:16, 1:24, 2:32, */
	
	uint16_t I2S_InterruptMask;		/*!< See in datesheet */
	
}I2S_InitTypeDef;
 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
void I2S_Init(I2S_InitTypeDef* I2S_InitStruct);  

/* Data transfers functions ***************************************************/
void I2S_Write(uint32_t LValue, uint32_t RValue);

//void I2S_Read(uint8_t * rBuf, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __JX_I2S_H__ */