/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_MMC_H__
#define __TEST_MMC_H__

#include "cdl_mmc.h"

#define SDMMC_MAX_TRAN_SPEED    25000000
#define ARM_CLK                 (400*1000*1000)
#define ARM_SD_CLK              (ARM_CLK/8)

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


int mmc_boot(unsigned long *entry_point);
int mmc_test(void);


#endif

