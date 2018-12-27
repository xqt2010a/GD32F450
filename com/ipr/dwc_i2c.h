#ifndef __DWC_I2C_H__
#define __DWC_I2C_H__

#include "config.h"

#define I2C_BASE(x)     (SYS_I2C_BASE+0x1000*(x))

#define I2C_CON(ch)              (*(volatile unsigned int *)(I2C_BASE(ch) + 0x00))
#define I2C_TAR(ch)              (*(volatile unsigned int *)(I2C_BASE(ch) + 0x04))
#define I2C_SAR(ch)              (*(volatile unsigned int *)(I2C_BASE(ch) + 0x08))
#define I2C_HS_MADDR(ch)         (*(volatile unsigned int *)(I2C_BASE(ch) + 0x0c))
#define I2C_DATA_CMD(ch)         (*(volatile unsigned int *)(I2C_BASE(ch) + 0x10))
#define I2C_SS_SCL_HCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x14))
#define I2C_UFM_SCL_HCNT(ch)     (*(volatile unsigned int *)(I2C_BASE(ch) + 0x14))
#define I2C_SS_SCL_LCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x18))
#define I2C_UFM_SCL_LCNT(ch)     (*(volatile unsigned int *)(I2C_BASE(ch) + 0x18))
#define I2C_FS_SCL_HCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x1c))

#define I2C_UFM_TBUF_CNT(ch)     (*(volatile unsigned int *)(I2C_BASE(ch) + 0x1c))
#define I2C_FS_SCL_LCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x20))
#define I2C_HS_SCL_HCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x24))
#define I2C_HS_SCL_LCNT(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x28))
#define I2C_INTR_STAT(ch)        (*(volatile unsigned int *)(I2C_BASE(ch) + 0x2c))
#define I2C_INTR_MASK(ch)        (*(volatile unsigned int *)(I2C_BASE(ch) + 0x30))
#define I2C_RAW_INTR_STAT(ch)    (*(volatile unsigned int *)(I2C_BASE(ch) + 0x34))
#define I2C_RX_TL(ch)            (*(volatile unsigned int *)(I2C_BASE(ch) + 0x38))
#define I2C_TX_TL(ch)            (*(volatile unsigned int *)(I2C_BASE(ch) + 0x3c))
#define I2C_CLR_INTR(ch)         (*(volatile unsigned int *)(I2C_BASE(ch) + 0x40))

#define I2C_CLRRX_UNDER(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x44))
#define I2C_CLRRX_OVER(ch)       (*(volatile unsigned int *)(I2C_BASE(ch) + 0x48))
#define I2C_CLRTX_OVER(ch)       (*(volatile unsigned int *)(I2C_BASE(ch) + 0x4c))
#define I2C_CLRRD_REQ(ch)        (*(volatile unsigned int *)(I2C_BASE(ch) + 0x50))
#define I2C_CLRTX_ABRT(ch)       (*(volatile unsigned int *)(I2C_BASE(ch) + 0x54))
#define I2C_CLRRX_DONE(ch)       (*(volatile unsigned int *)(I2C_BASE(ch) + 0x58))
#define I2C_CLRACTIVITY(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x5c))
#define I2C_CLRSTOP_DET(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x60))
#define I2C_CLRSTART_DET(ch)     (*(volatile unsigned int *)(I2C_BASE(ch) + 0x64))
#define I2C_CLRGEN_CALL(ch)      (*(volatile unsigned int *)(I2C_BASE(ch) + 0x68))

#define I2C_ENABLE(ch)                   (*(volatile unsigned int *)(I2C_BASE(ch) + 0x6c))
#define I2C_STATUS(ch)                   (*(volatile unsigned int *)(I2C_BASE(ch) + 0x70))
#define I2C_TXFLR(ch)                    (*(volatile unsigned int *)(I2C_BASE(ch) + 0x74))
#define I2C_RXFLR(ch)                    (*(volatile unsigned int *)(I2C_BASE(ch) + 0x78))
#define I2C_SDA_HOLD(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0x7c))
#define I2C_TX_ABRT_SOURCE(ch)           (*(volatile unsigned int *)(I2C_BASE(ch) + 0x80))
#define I2C_SLV_DATA_NACK_ONLY(ch)       (*(volatile unsigned int *)(I2C_BASE(ch) + 0x84))
#define I2C_DMA_CR(ch)                   (*(volatile unsigned int *)(I2C_BASE(ch) + 0x88))
#define I2C_DMA_TDLR(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0x8c))
#define I2C_DMA_RDLR(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0x90))

#define I2C_SDA_SETUP(ch)                (*(volatile unsigned int *)(I2C_BASE(ch) + 0x94))
#define I2C_ACK_GENERAL_CALL(ch)         (*(volatile unsigned int *)(I2C_BASE(ch) + 0x98))
#define I2C_ENABLE_STATUS(ch)            (*(volatile unsigned int *)(I2C_BASE(ch) + 0x9c))
#define I2C_FS_SPKLEN(ch)                (*(volatile unsigned int *)(I2C_BASE(ch) + 0xa0))
#define I2C_UFM_SPKLEN(ch)               (*(volatile unsigned int *)(I2C_BASE(ch) + 0xa0))
#define I2C_HS_SPKLEN(ch)                (*(volatile unsigned int *)(I2C_BASE(ch) + 0xa4))
#define I2C_CLR_RESTART_DET(ch)          (*(volatile unsigned int *)(I2C_BASE(ch) + 0xa8))
#define I2C_SCL_STUCK_AT_LOW_TIMROUT(ch) (*(volatile unsigned int *)(I2C_BASE(ch) + 0xac))
#define I2C_SDA_STUCK_AT_LOW_TIMEOUT(ch) (*(volatile unsigned int *)(I2C_BASE(ch) + 0xb0))
#define I2C_CLR_SCL_STUCK_DET(ch)        (*(volatile unsigned int *)(I2C_BASE(ch) + 0xb4))

#define I2C_DEVICE_ID(ch)                    (*(volatile unsigned int *)(I2C_BASE(ch) + 0xb8))
#define I2C_SMBUS_CLK_LOW_SEXT(ch)           (*(volatile unsigned int *)(I2C_BASE(ch) + 0xbc))
#define I2C_SMBUS_CLK_LOW_MEXT(ch)           (*(volatile unsigned int *)(I2C_BASE(ch) + 0xc0))
#define I2C_SMBUS_THIGH_MAX_IDLE_COUNT(ch)   (*(volatile unsigned int *)(I2C_BASE(ch) + 0xc4))
#define I2C_SMBUS_INTR_STAT(ch)              (*(volatile unsigned int *)(I2C_BASE(ch) + 0xc8))
#define I2C_SMBUS_INTR_MASK(ch)              (*(volatile unsigned int *)(I2C_BASE(ch) + 0xcc))
#define I2C_SMBUS_RAW_INTR_STAT(ch)          (*(volatile unsigned int *)(I2C_BASE(ch) + 0xd0))
#define I2C_CLR_SMBUS_INTR(ch)               (*(volatile unsigned int *)(I2C_BASE(ch) + 0xd4))
#define I2C_OPTIONAL_SAR(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0xd8))
#define I2C_SMBUS_UDID_LSB(ch)               (*(volatile unsigned int *)(I2C_BASE(ch) + 0xdc))

#define I2C_COMP_PARAM_1(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0xf4))
#define I2C_COMP_VERSION(ch)                 (*(volatile unsigned int *)(I2C_BASE(ch) + 0xf8))
#define I2C_COMP_TYPE(ch)                    (*(volatile unsigned int *)(I2C_BASE(ch) + 0xfc))

/**
  * @brief  I2C Init structure definition
  */
  
typedef struct
{
    uint8_t ch;
    uint8_t mode:4;     //0:master 1:slave
    uint8_t addr_mode:4;//0:7bit 1:10bit
    uint16_t int_mask;  //see datasheet
    
    uint32_t clk;       //i2c module clk
    uint32_t speed;     //stand, fast, high
    uint32_t own_addr;  //target addr
} I2C_InitTypeDef;

void dwc_i2c_init(I2C_InitTypeDef* i2c);
void dwc_i2c_write(I2C_InitTypeDef* i2c, unsigned char * wBuf, unsigned char len);
void dwc_i2c_read(I2C_InitTypeDef* i2c, unsigned char * rBuf, unsigned char len);


#endif  /* __DWC_I2C_H__ */