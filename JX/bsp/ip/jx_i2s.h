#ifndef __JX_I2S_H__
#define __JX_I2S_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
    
#define I2S_BASE0       0x3ff28000
#define I2S_BASE1       0x3ff29000
#define I2S_BASE2       0x3ff2a000
//#define I2S_BASE        I2S_BASE0

/* ------------------------  I2S REG DEF -----------------------------------  */
#define I2S_IER(I2S_BASEn)    	           (*(volatile unsigned long*)(I2S_BASEn + 0x000))   //DW_apb_i2s Enable Register
#define I2S_IRER(I2S_BASEn) 		   (*(volatile unsigned long*)(I2S_BASEn + 0x004))   //I2S Receiver Block Enable Register
#define I2S_ITER(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x008))   //I2S Transmitter Block Enable Register
#define I2S_CER(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x00C))   //Clock Enable Register
#define I2S_CCR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x010))   //Clock Configuration Register
#define I2S_RXFFR(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x014))   //Receiver Block FIFO Register   
#define I2S_TXFFR(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x018))   //Transmitter Block FIFO Register
    
#define I2S_LRBR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x020))   //Left Receive Buffer 0
#define I2S_LTHR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x020))   //Left Transmit Holding Register 0
#define I2S_RRBR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x024))   //Right Receive Buffer 0
#define I2S_RTHR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x024))   //Right Transmit Holding Register 0
#define I2S_RER(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x028))   //Receive Enable Register 0
#define I2S_TER(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x02C))   //Transmit Enable Register 0
#define I2S_RCR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x030))   //Receive Configuration Register 0
#define I2S_TCR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x034))   //Transmit Configuration Register 0
#define I2S_ISR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x038))   //Interrupt Status Register 0
#define I2S_IMR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x03C))   //Interrupt Mask Register 0
#define I2S_ROR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x040))   //Receive Overrun Register 0
#define I2S_TOR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x044))   //Transmit Overrun Register 0
#define I2S_RFCR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x048))   //Receive FIFO Configuration Register 0
#define I2S_TFCR(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x04C))   //Transmit FIFO Configuration Register 0
#define I2S_RFF(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x050))   //Receive FIFO Flush 0
#define I2S_TFF(I2S_BASEn)    		   (*(volatile unsigned long*)(I2S_BASEn + 0x054))   //Transmit FIFO Flush 0
    
#define I2S_RXDMA(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x1C0))   //Receiver Block DMA Register
#define I2S_RRXDMA(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x1C4))   //Reset Receiver Block DMA Register
#define I2S_TXDMA(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x1C8))   //Transmitter Block DMA Register
#define I2S_RTXDMA(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x1CC))   //Reset Transmitter Block DMA Register
#define I2S_COMP_PARAM_2(I2S_BASEn)        (*(volatile unsigned long*)(I2S_BASEn + 0x1F0))   //Component Parameter 2 Register
#define I2S_COMP_PARAM_1(I2S_BASEn)        (*(volatile unsigned long*)(I2S_BASEn + 0x1F4))   //Component Parameter 1 Register
#define I2S_COMP_VERSION(I2S_BASEn)        (*(volatile unsigned long*)(I2S_BASEn + 0x1F8))   //Component Version ID
#define I2S_COMP_TYPE(I2S_BASEn)    	   (*(volatile unsigned long*)(I2S_BASEn + 0x1FC))   //DesignWare Component Type
    
/**
  * @brief  I2S Init structure definition
  */

typedef struct
{
    uint32_t base_addr;                 /* ch = 0, 1, 2*/
    uint8_t I2S_BitRlt;                 /*!< bit resolution 1:12b, 2:16b, 3:20b, 4:24b, 5:32b */
	
    uint32_t I2S_Sclk;                  /*!< sample_fs * 2 * bit resolution*/
	
    uint32_t I2S_Mclk;                  /*!< sample_fs * 256 */
    uint8_t I2S_ClkCyc;                 /*!< Clock cycles 0:16, 1:24, 2:32, */
	
    uint16_t I2S_InterruptMask;		/*!< See in datesheet */
	
}I2S_InitTypeDef;
 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
void I2S_Init(I2S_InitTypeDef* I2S_InitStruct);  

/* Data transfers functions ***************************************************/
void I2S_Write(I2S_InitTypeDef* I2S_InitStruct, uint32_t LValue, uint32_t RValue);

//void I2S_Read(uint8_t * rBuf, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __JX_I2S_H__ */