#ifndef __GD25Q64_H__
#define __GD25Q64_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "jx_spi.h"

#define GD25Q64C_PAGE_SIZE    256           /* 每一页最多可以写256字节 W25X64共有8192*4页 */
#define GD25Q64C_SECTOR_SIZE        4*1024  	/* 4K byte = 4KB */
#define GD25Q64C_BLOCK_SIZE_32KB   32*1024
#define GD25Q64C_BLOCK_SIZE_64KB   64*1024

#define FLASH_WREN    0x06    /* 写使能 */
#define FLASH_WRDI    0x04    /* 关写使能 */
#define FLASH_VSWN    0x50
#define FLASH_RSR1    0x05
#define FLASH_RSR2    0x35
#define FLASH_RSR3    0x15
#define FLASH_WSR1    0x01
#define FLASH_WSR2    0x31
#define FLASH_WSR3    0x11
#define FLASH_READ    0x03
#define FLASH_FREAD   0x0B
#define FLASH_DOFR    0x3B
#define FLASH_QOFR    0x6B
#define FLASH_DIOFR   0xBB
#define FLASH_QIOFR   0xEB
#define FLASH_QIOWFR  0xE7
#define FLASH_SBW     0x77
#define FLASH_PP      0x02
#define FLASH_QPP     0x32
#define FLASH_FPP     0xF2
#define FLASH_SE      0x20  /* Sector Erase(4K) */
#define FLASH_BE32    0x52	/* Block Erase(32K) */
#define FLASH_BE64    0xD8	/* Block Erase(64K) */
#define FLASH_CE1     0x60 	/* Chip Erase */
#define FLASH_CE2     0xC7	/* Chip Erase */
#define FLASH_DP      0xB9
#define FLASH_RDI     0xAB
#define FLASH_REMS    0x90
#define FLASH_DREMS   0x92
#define FLASH_QREMS   0x94
#define FLASH_RDID    0x9F    /* 读取ID命令 */
#define FLASH_HPM     0xA3
#define FLASH_PES     0x75
#define FLASH_PER     0x7A
#define FLASH_ESR     0x44
#define FLASH_PSR     0x42
#define FLASH_RSR     0x48
#define FLASH_ER      0x66
#define FLASH_RST     0x99
#define FLASH_RSFDP   0x5A

#define FLASH_DUMMY_BYTE   0xA5  /*空字节*/
#define FLASH_WIP_FLAG     0x01  /*!< Write In Progress (WIP) flag */
#define FLASH_SET_WIP_FLAG 0x01  /*!< Set Write In Progress (WIP) flag */
  
/* ******************************** functions *********************************/
void SPIM1_Flash_Write_Enable(void);  //写使能
void SPIM1_Flash_Write_Disable(void);  //关闭写使能
uint32_t SPIM1_Flash_Read_ID(void);  //读 FlashID
void SPIM1_Flash_Wait(void);  //等待写，擦完成
void SPIM1_Flash_Page_Write(uint8_t *wBuffer, uint32_t WriteAddr, uint16_t N_Byte); //Max Byte 256
void SPIM1_Flash_Page_Read(uint8_t *rBuffer, uint32_t ReadAddr, uint16_t N_Byte); //Max Byte 256
void SPIM1_Flash_Erase(uint8_t FlashCmd, uint32_t WriteAddr);  //擦除 Flash 

#ifdef __cplusplus
}
#endif

#endif  /* __GD25Q64_H__ */
