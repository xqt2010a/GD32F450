#ifndef __JX_SPI_H__
#define __JX_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_BASE0   0x3ff20000
#define SPI_BASE1   0x3ff21000
#define SPI_BASE2   0x3ff22000
#define SPI_BASE3   0x3ff23000
    
#define SSIM1_BASE   SPI_BASE3
    
/* ------------------------  SSIM1 REG DEF ---------------------------------  */
#define SSIM1_CTRLR0    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x00))   	//Control Register 0
#define SSIM1_CTRLR1    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x04))   	//Control Register 1
#define SSIM1_SSIENR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x08))   	//SSI Enable Register
#define SSIM1_MWCR      		 (*(volatile unsigned long*)(SSIM1_BASE + 0x0C))   	//Microwire Control Register
#define SSIM1_SER       		 (*(volatile unsigned long*)(SSIM1_BASE + 0x10))   	//Slave Enable Register
#define SSIM1_BAUDR     		 (*(volatile unsigned long*)(SSIM1_BASE + 0x14))   	//Baud Rate Select
#define SSIM1_TXFTLR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x18))   	//Transmit FIFO Threshold Level
#define SSIM1_RXFTLR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x1C))   	//Receive FIFO Threshold Level
#define SSIM1_TXFLR     		 (*(volatile unsigned long*)(SSIM1_BASE + 0x20))   	//Transmit FIFO Level Register
#define SSIM1_RXFLR     		 (*(volatile unsigned long*)(SSIM1_BASE + 0x24))   	//Receive FIFO Level Register
#define SSIM1_SR        		 (*(volatile unsigned long*)(SSIM1_BASE + 0x28))   	//Status Register
#define SSIM1_IMR       		 (*(volatile unsigned long*)(SSIM1_BASE + 0x2C))   	//Interrupt Mask Register
#define SSIM1_ISR       		 (*(volatile unsigned long*)(SSIM1_BASE + 0x30))   	//Interrupt Status Register
#define SSIM1_RISR      		 (*(volatile unsigned long*)(SSIM1_BASE + 0x34))   	//Raw Interrupt Status Register
#define SSIM1_TXOICR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x38))   	//Transmit FIFO Overflow Interrupt Clear Register
#define SSIM1_RXOICR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x3C))   	//Receive FIFO Overflow Interrupt Clear Register
#define SSIM1_RXUICR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x40))   	//Receive FIFO Underflow Interrupt Clear Register
#define SSIM1_MSTICR    		 (*(volatile unsigned long*)(SSIM1_BASE + 0x44))   	//Multi-Master Interrupt Clear Register
#define SSIM1_ICR       		 (*(volatile unsigned long*)(SSIM1_BASE + 0x48))   	//Interrupt Clear Register
#define SSIM1_DMACR     		 (*(volatile unsigned long*)(SSIM1_BASE + 0x4C))   	//DMA Control Register
#define SSIM1_DMATDLR   		 (*(volatile unsigned long*)(SSIM1_BASE + 0x50))   	//DMA Transmit Data Level
#define SSIM1_DMARDLR   		 (*(volatile unsigned long*)(SSIM1_BASE + 0x54))   	//DMA Receive Data Level
#define SSIM1_IDR       		 (*(volatile unsigned long*)(SSIM1_BASE + 0x58))   	//Identification Register
#define SSIM1_COMP_VERSION   	 (*(volatile unsigned long*)(SSIM1_BASE + 0x5C))   	//coreKit version ID register
#define SSIM1_DR        		 (*(volatile unsigned long*)(SSIM1_BASE + 0x60))   	//Data Register
#define SSIM1_RX_SAMPLE_DLY      (*(volatile unsigned long*)(SSIM1_BASE + 0xf0))   	//RXD Sample Delay Register
    
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/** 
  * @brief  SPI Init structure definition  
  */

typedef struct
{
  uint16_t SPI_Direction;           /*!< Specifies the SPI unidirectional or bidirectional data mode.
                                         This parameter can be a value of @ref SPI_data_direction */

  uint16_t SPI_Mode;                /*!< Specifies the SPI mode (Master/Slave).
                                         This parameter can be a value of @ref SPI_mode */
  
  uint16_t SPI_DataSize;            /*!< Specifies the SPI data size.
                                         This parameter can be a value of @ref SPI_data_size */

  uint16_t SPI_CPOL;                /*!< Specifies the serial clock steady state.
                                         This parameter can be a value of @ref SPI_Clock_Polarity */

  uint16_t SPI_CPHA;                /*!< Specifies the clock active edge for the bit capture.
                                         This parameter can be a value of @ref SPI_Clock_Phase */

  uint16_t SPI_NSS;                 /*!< Specifies whether the NSS signal is managed by
                                         hardware (NSS pin) or by software using the SSI bit.
                                         This parameter can be a value of @ref SPI_Slave_Select_management */
 
  uint16_t SPI_BaudRatePrescaler;   /*!< Specifies the Baud Rate prescaler value which will be
                                         used to configure the transmit and receive SCK clock.
                                         This parameter can be a value of @ref SPI_BaudRate_Prescaler
                                         @note The communication clock is derived from the master
                                               clock. The slave clock does not need to be set. */

  uint16_t SPI_FirstBit;            /*!< Specifies whether data transfers start from MSB or LSB bit.
                                         This parameter can be a value of @ref SPI_MSB_LSB_transmission */

  uint16_t SPI_CRCPolynomial;       /*!< Specifies the polynomial used for the CRC calculation. */
}SPI_InitTypeDef;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
//void SPI_Init(SPI_InitTypeDef* SPI_InitStruct);
void SPIM1_Init(void);
/* Data transfers functions ***************************************************/
void SPI_Read_ID(void);
void SPIM1_Write_Read(uint8_t * wBuf, uint32_t wLen, uint8_t * rBuf, uint32_t rLen);

#ifdef __cplusplus
}
#endif


#endif  /* __JX_SPI_H__ */