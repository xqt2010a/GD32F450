/*
 * iic.h
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: cnan
 */

#ifndef __I2C_H__
#define __I2C_H__

#include "stdint.h"

#define I2C_DEBUG	0

#define DUMMY_WRITE 0xA5

#define I2C_BASE	0x3FF00000

#define I2C0_BASE	0x3FF00000
#define I2C1_BASE	0x3FF01000
#define I2C2_BASE	0x3FF02000
#define I2C3_BASE	0x3FF03000
#define I2C4_BASE	0x3FF04000
#define I2C5_BASE	0x3FF05000
#define I2C6_BASE	0x3FF06000
#define I2C7_BASE	0x3FF07000
#define I2C8_BASE	0x3FF08000
#define I2C9_BASE	0x3FF09000
#define I2C10_BASE	0x3FF0a000
#define I2C11_BASE	0x3FF0b000
#define I2C12_BASE	0x3FF0c000
#define I2C13_BASE	0x3FF0d000
#define I2C14_BASE	0x3FF0e000
#define I2C15_BASE	0x3FF0f000

#define __I		volatile const	/* read only */
#define __O 	volatile		/* write only */
#define __IO	volatile		/* read/write */

#define	I2C_CON(x)							(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x00))
#define I2C_TAR(x)							(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x04))
#define I2C_SAR(x)							(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x08))
#define I2C_HS_MADDR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x0c))
#define I2C_DATA_CMD(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x10))
#define I2C_SS_SCL_HCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x14))
#define I2C_UFM_SCL_HCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x14))
#define	I2C_SS_SCL_LCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x18))
#define	I2C_UFM_SCL_LCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x18))
#define	I2C_FS_SCL_HCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x1c))
				
#define	I2C_UFM_TBUF_CNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x1c))
#define	I2C_FS_SCL_LCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x20))
#define	I2C_HS_SCL_HCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x24))
#define	I2C_HS_SCL_LCNT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x28))
#define	I2C_INTR_STAT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x2c))
#define	I2C_INTR_MASK(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x30))
#define	I2C_RAW_INTR_STAT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x34))
#define	I2C_RX_TL(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x38))
#define	I2C_TX_TL(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x3c))
#define	I2C_CLR_INTR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x40))
				
#define	I2C_CLRRX_UNDER(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x44))
#define	I2C_CLRRX_OVER(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x48))
#define	I2C_CLRTX_OVER(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x4c))
#define	I2C_CLRRD_REQ(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x50))
#define	I2C_CLRTX_ABRT(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x54))
#define	I2C_CLRRX_DONE(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x58))
#define	I2C_CLRACTIVITY(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x5c))
#define	I2C_CLRSTOP_DET(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x60))
#define	I2C_CLRSTART_DET(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x64))
#define	I2C_CLRGEN_CALL(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x68))
				
#define	I2C_ENABLE(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x6c))
#define	I2C_STATUS(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x70))
#define	I2C_TXFLR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x74))
#define	I2C_RXFLR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x78))
#define	I2C_SDA_HOLD(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x7c))
#define	I2C_TX_ABRT_SOURCE(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x80))
#define	I2C_SLV_DATA_NACK_ONLY(x)			(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x84))
#define	I2C_DMA_CR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x88))
#define	I2C_DMA_TDLR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x8c))
#define	I2C_DMA_RDLR(x)						(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x90))

#define	I2C_SDA_SETUP(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x94))
#define	I2C_ACK_GENERAL_CALL(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x98))
#define	I2C_ENABLE_STATUS(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0x9c))
#define	I2C_FS_SPKLEN(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xa0))
#define	I2C_UFM_SPKLEN(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xa0))
#define	I2C_HS_SPKLEN(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xa4))
#define	I2C_CLR_RESTART_DET(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xa8))
#define	I2C_SCL_STUCK_AT_LOW_TIMROUT(x)		(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xac))
#define	I2C_SDA_STUCK_AT_LOW_TIMEOUT(x)		(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xb0))
#define	I2C_CLR_SCL_STUCK_DET(x)			(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xb4))

#define	I2C_DEVICE_ID(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xb8))
#define	I2C_SMBUS_CLK_LOW_SEXT(x)			(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xbc))
#define	I2C_SMBUS_CLK_LOW_MEXT(x)			(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xc0))
#define	I2C_SMBUS_THIGH_MAX_IDLE_COUNT(x)	(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xc4))
#define	I2C_SMBUS_INTR_STAT(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xc8))
#define	I2C_SMBUS_INTR_MASK(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xcc))
#define	I2C_SMBUS_RAW_INTR_STAT(x)			(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xd0))
#define	I2C_CLR_SMBUS_INTR(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xd4))
#define	I2C_OPTIONAL_SAR(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xd8))
#define	I2C_SMBUS_UDID_LSB(x)				(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xdc))
        
#define	I2C_COMP_PARAM_1(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xf4))
#define	I2C_COMP_VERSION(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xf8))
#define	I2C_COMP_TYPE(x)					(*(volatile unsigned int *)(I2C_BASE + 0x1000 * x + 0xfc))

//I2C Port Number
typedef enum
{
	I2C0	= 0,
	I2C1	= 1,
	I2C2	= 2,
	I2C3	= 3,
	I2C4	= 4,
	I2C5	= 5,
	I2C6	= 6,
	I2C7	= 7,
	I2C8	= 8,
	I2C9	= 9,
	I2C10	= 10,
	I2C11	= 11,
	I2C12	= 12,
	I2C13	= 13,
	I2C14	= 14,
	I2C15	= 15
}I2C_PORT;

/**
  * @brief  I2C Init structure definition
  */
  
typedef struct
{
  	unsigned char I2C_Mode;               /*!< Specifies the I2C mode. 0:Master 1:Slave */
	
	unsigned char I2C_Speed;				/*!< Specifies the I2C I2C_Speed. 0:Standard 1:fast 2:high */
	
	unsigned char I2C_AddrMode;           /*!< Specifies the device own address mode. 0:7-bit address 1:10-bit address*/
	
	unsigned char I2C_OwnAddress;         /*!< Specifies the device own address. */

	unsigned short I2C_InterruptMask;		/*!< See in datesheet */
	
} I2C_InitTypeDef;

typedef enum
{
	I2C_MASTER	= 0,
	I2C_SLAVE	= 1
}I2C_InitMode;

typedef enum
{
	I2C_STANDARD	= 0,
	I2C_FAST		= 1,
	I2C_HIGH		= 2	
}I2C_InitSpeed;

typedef enum
{
	I2C_7BIT	= 0,
	I2C_10BIT	= 1
}I2C_InitAddrMode;



/* Initialization and Configuration functions *********************************/
void I2C_Init(unsigned char ch, I2C_InitTypeDef* I2C_InitS);  /* ch=0~15 */

/* Data transfers functions ***************************************************/
void I2C_Write(unsigned char ch, uint8_t * wBuf, uint8_t len);

void I2C_Read(unsigned char ch, uint8_t * rBuf, uint8_t len);

uint16_t read_lm75a(unsigned char ch);

#endif /* __I2C_H__ */
