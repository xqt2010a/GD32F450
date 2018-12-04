/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_MMC_H__
#define __TEST_MMC_H__

#include "cdl_mmc.h"

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

#define IMAGE_LOAD_SUCESS (0)
#define IMAGE_LOAD_FAIL	  (1)

// define the image header struct
typedef struct _img_header_t
{
	unsigned long fw_marker;			// firmware marker(fixed to be 0x5AA55AA5)
										// = IMG_FW_MARKER
	unsigned long lba_cnt;           	// sector count of image store used

	unsigned long ins_addr;				// instruction address on ram
#ifdef CHECKSUM_SUPPORT 
	unsigned long header_checksum;		// the checksum of image header
	unsigned long image_checksum;		// the checksum of image data
#endif	
	unsigned char  fw_ver_maj;			// firmware version
	unsigned char  fw_ver_min;	
} img_header_t;

void mmc_init(void);
int mmc_boot(unsigned long *entry_point);
void mmc_test(void);


#endif

