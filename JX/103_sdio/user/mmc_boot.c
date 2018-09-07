/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2018 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "mmc_boot.h"
//#include "pll.h"
#include "mmc.h"
//#include "sys.h"
//#include "uart.h"
#include "data_type.h"
#include "stdio.h"



struct dwmci_host mmc_host;
struct dwmci_host* p_mmc_host = &mmc_host;
struct mmc* p_mmc_dev = NULL;

#define LAB_0 					(0)
#define SECTOR_SIZE 			(512)
#define IMG_FW_MARKER			(0x5AA55AA5) 
#if defined(ASIC_VERSION)
/*SRAM:128k, 64k for load the image, 64k for bootrom .data .bss and .stack*/
#define START_SRAM_ADDR_FOR_SPL	(0x00080000)
#define END_SRAM_ADDR_FOR_SPL	(0x00090000)
#else
#define START_SRAM_ADDR_FOR_SPL	(0x10000000)
#define END_SRAM_ADDR_FOR_SPL	(0x48000000)  //0x10010000
#endif

#if defined(ASIC_VERSION)
#define SD0_BASE		0x02408000
#else
#define SD0_BASE		0xC1600000
#endif
static unsigned char swap_dbuf[SECTOR_SIZE];

static int image_header_check_parse(img_header_t *p_hdr)
{
	//mark checking
	if(p_hdr->fw_marker != IMG_FW_MARKER)
	{
		return -1;
	}	

	//image_len checking
	if(	((p_hdr->lba_cnt<<9) + p_hdr->ins_addr) > END_SRAM_ADDR_FOR_SPL ||
		p_hdr->ins_addr < START_SRAM_ADDR_FOR_SPL ||
		(p_hdr->ins_addr % 4) != 0)
	{
		debug("invalid image header\r\n");
		return -2;
	}	

	//header checksum checking
#ifdef CHECKSUM_SUPPORT

#endif

	return 0;	
}

static int mmc_init(unsigned int dma_mode, unsigned long speed, unsigned long bus_width)
{
	int ret;

#if 0
	// very important to ASIC, please make sure these pin mux is done by NextVPU
#ifndef ASIC_VERSION
	REG32(0x20004000 + 0xA0) = 0x0;
	REG32(0x20004000 + 0xC0) = 0x03ffffff;
	REG32(0x20004000 + 0xD0) = 0x0;
	REG32(0x20004000 + 0xF0) = 0x0;
#endif
#endif
	p_mmc_host->fifo_mode = !dma_mode;
	dw_mmc_init(p_mmc_host,	(unsigned int)p_mmc_host->regs, p_mmc_host->pclk, bus_width, speed);

	/*obtain mmc device structure*/
	p_mmc_dev = mmc_create((const struct mmc_config *)&p_mmc_host->cfg, p_mmc_host);
	p_mmc_dev->user_speed = speed;
	p_mmc_dev->user_bus_width = bus_width;
	p_mmc_dev->sys_max_tran_speed = SDMMC_MAX_TRAN_SPEED;

	ret = mmc_init_card(p_mmc_dev);
	if (ret) {
		debug("card init fail\r\n");
		return 1;
	}

	/*print card information*/
	if (IS_SD(p_mmc_dev)) {
		debug("SD card, version: %d.%d\r\n",
			(p_mmc_dev->version>>16)&0xff,
			(p_mmc_dev->version>>8)&0xff);
	}
	if (IS_MMC(p_mmc_dev)) {
		debug("MMC card, version: %d.%d\r\n",
			(p_mmc_dev->version>>16)&0xff,
			(p_mmc_dev->version>>8)&0xff);
	}

	debug("card high speed mode: %s\r\n",(p_mmc_dev->card_caps & MMC_MODE_HS)?"support":"not support");
	if (IS_MMC(p_mmc_dev)) {
		debug("card high speed 52M mode: %s\r\n",(p_mmc_dev->card_caps & MMC_MODE_HS_52MHz)?"support":"not support");
	}
	debug("card bus width support: 1 %s %s bit\r\n",
		(p_mmc_dev->card_caps & MMC_MODE_4BIT)?"4":"-",
		(p_mmc_dev->card_caps & MMC_MODE_8BIT)?"8":"-");

	debug("total capacity: 0x%x M max lba=%d block len=%d\r\n",
		(p_mmc_dev->capacity/1024)/1024,
		p_mmc_dev->block_dev.lba,
		p_mmc_dev->read_bl_len);
	debug("\r\n");
	dwmci_display_config(p_mmc_host);

	return 0;
}

/*****************************************************************************/
//  Function:  mmc_boot(void *entry_point)
//
//  Parameters:
//		entry_point		: valid image jump address
//	
//  Return:
//		0				: success
//
//		none 0			: fail
//		
//  Description:
//      load a valid image from the sd media.
//
/*****************************************************************************/
int mmc_boot(unsigned long *entry_point)
{
	img_header_t *p_hdr = (img_header_t *)swap_dbuf;

	memset(p_mmc_host, 0, sizeof(struct dwmci_host));
	dw_mmc_init(p_mmc_host, SD0_BASE, ARM_SD_CLK, 4, 25000000);

	if(mmc_init(0, 25000000, 4)) {
		debug("sd card init fail\r\n");
		goto __load_err;
	}
	/* Read the 1st sector (LBA 0, must be 512B) to image header buffer*/
	if (mmc_bread(LAB_0, 1, (char*)swap_dbuf)!= 1) {
		debug("read image content fail\r\n");
		goto __load_err;
	}

    if (image_header_check_parse((img_header_t *)swap_dbuf) != 0) {
    	debug("check header fail\r\n");
    	goto __load_err;
	}

	if (mmc_bread(LAB_0+1, p_hdr->lba_cnt, (char*)p_hdr->ins_addr) != p_hdr->lba_cnt) {
		debug("load image content fail\r\n");
		goto __load_err;
	}

	debug("image loading success!\r\n");
	*entry_point =  p_hdr->ins_addr;
	return IMAGE_LOAD_SUCESS;
__load_err:
    debug("\r\nimage loading failed!\r\n");
	return IMAGE_LOAD_FAIL;
}

int mmc_test(void)
{
	img_header_t *p_hdr = (img_header_t *)swap_dbuf;

	memset(p_mmc_host, 0, sizeof(struct dwmci_host));
	dw_mmc_init(p_mmc_host, SD0_BASE, ARM_SD_CLK, 4, 25000000);

	if(mmc_init(0, 25000000, 4)) {
		debug("sd card init fail\r\n");
	}
	/* Read the 1st sector (LBA 0, must be 512B) to image header buffer*/
	if (mmc_bread(LAB_0, 1, (char*)swap_dbuf)!= 1) {
		debug("read image content fail\r\n");
	}

    if (image_header_check_parse((img_header_t *)swap_dbuf) != 0) {
    	debug("check header fail\r\n");
	}



	return IMAGE_LOAD_SUCESS;
}

