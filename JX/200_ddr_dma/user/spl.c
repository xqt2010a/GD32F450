#include "spl.h"
#include "ddr.h"

int jiaxing_dwmci_add_port(int, u32, int);

static int spl_mmc_init(void)
{
	int ret = 0;

	/* TODO: MMC PLL/Clock/Reset/Pinmux/Pad initialization. */
	ret = jiaxing_dwmci_add_port(0, SD0_BASE, 4);

	if(ret) {
		puts("[SPL_BOARD_INIT] SPL mmc init failed.\n");
		return -1;
	}

	return 0;
}

u32 spl_boot_device()
{
	/* NORMINATE the first order boot device. */
	return BOOT_DEVICE_MMC1;
}

static u32 spl_load_mmc_raw(int start, int size, void* buf)
{
	struct spl_image_info spl_image;
	struct spl_boot_device spl_bootdev;
	
	spl_bootdev.boot_device = spl_boot_device();
	memset(&spl_image, '\0', sizeof(spl_image));
	spl_image.load_addr = start;
	spl_image.size = size;
	spl_image.arg = buf; /* read back buffer address for custom load. */
	spl_image.os = 0xFF; /* handshake token as for a custom load. */
	if (spl_mmc_load_image(&spl_image, &spl_bootdev)) {
		puts("SPL: failed to load custom images in raw sectors.\n");
		hang();
	}
	return spl_image.size; /* size is updated in load program. */
}

/* 
 * Please note that the start address here should be always 512B aligned. 
 * We don't handle this complexity in low level load codes.
 * image length is supposed to be 4byte aligned, better to be 512B aligned. 
 */
static int spl_load_image(int start, int loadmode, int verify)
{
	u32 *imagebuf;
	u32 _start, dest, size, tmpidx;
	u32 buflen = SPL_LOADIMAGE_BUF_LEN; /*SHOULD BE 512B aligned*/
	imagebuf = (u32*) SPL_LOADIMAGE_BUF_ADDR;
	if(spl_load_mmc_raw(start*512, 512, (void*)imagebuf) != 512){
		puts("SPL: failed to load custom images.\n");
		return -1;
	}
	if(imagebuf[0] != IMAGE_MAGIC) {
		puts("SPL: invalid custom images.\n");
	}
	_start = imagebuf[1];
	dest = imagebuf[2];
	size = imagebuf[3];

	if(_start%512 != 0){
		puts("SPL: custom image load start address is not 512B aligned.\n");
		return -1;
	}
	
	tmpidx = 0;
	while (size > 0) {
		u32 i,tmplen, tmp32, pr_to32;
		
		tmplen = (size>buflen)?buflen:size;
		spl_load_mmc_raw(_start+tmpidx,tmplen,(void*)imagebuf);
		if(loadmode == 1) { /* DDR load */
			/* TODO: We need a double check on ARC SRAM step length. a short access leads to 2B step or a 4B step? 
			 *       In implementation below, we use 4.
			 */
#define ARC_RAM_ACCESS_STEPLEN	4
			pr_to32 =  dest + tmpidx*ARC_RAM_ACCESS_STEPLEN; 
			for(i = 0x0; i < tmplen/4; i++){
				tmp32 = imagebuf[i];
				reg16_write(pr_to32, tmp32 & 0x0000ffff);
				pr_to32 += ARC_RAM_ACCESS_STEPLEN; 
				reg16_write(pr_to32, (tmp32 >> 16) & 0x0000ffff);
				pr_to32 += ARC_RAM_ACCESS_STEPLEN; 
			}			
		} else {
			memcpy((void*)imagebuf, (void*) (dest + tmpidx), tmplen);
		}
		if(verify){
			if(loadmode == 1) {
				pr_to32 =  dest + tmpidx*ARC_RAM_ACCESS_STEPLEN; 
				for(i = 0x0; i < tmplen/4;i++){
					tmp32 = (reg16_read(pr_to32) & 0x0000ffff);
					pr_to32 += ARC_RAM_ACCESS_STEPLEN;
					tmp32 += ((reg16_read(pr_to32) & 0x0000ffff) << 16);
			
					if(tmp32 != imagebuf[i]){
						puts("SPL: image verify failed.\n");
						return -1;
					}
					pr_to32 += ARC_RAM_ACCESS_STEPLEN;
				}
			}else {
				for(i=0;i<tmplen/4;i++) {
					if(imagebuf[i]!=((u32*)dest)[i]){
						puts("SPL: image verify failed.\n");
						return -1;
					}
				}
			}
		}
		size -= tmplen;	
		tmpidx += tmplen;
	}
	return 0;
}

u32 ddr_traincode_loadinfo[2][2] = {
	{0x100, 0x180}, /* 1D: IMEM,  DMEM  */
	{0x200, 0x280}  /* 2D:  IMEM, DMEM */
};

int ddr_load_train_code(int type)
{
	int err;
	err = spl_load_image(ddr_traincode_loadinfo[type][0], 1, 0);
	if(err) return err;
	err = spl_load_image(ddr_traincode_loadinfo[type][1], 1, 0);
	if(err) return err;
	return 0;
}

void spl_board_init()
{
	/* Clock/Reset init.*/
#if 1	/*
	 * Got it from Zane which is proved to make UART work in his test code.
	 * TODO: To make it more readable in code cleanup stage.
	 */
	writel(0xFFFFFFFF, 0x3FE08100);
	writel(0xFFFFFFFF, 0x3FE08104);
	writel(0xFFFFFFFF, 0x0190d100);
	writel(0xFFFFFFFF, 0x0190d104);
	writel(0xFFFFFFFF, 0x0190d108);
	writel(0xFFFFFFFF, 0x0190d10c);
	writel(0xFFFFFFFF, 0x0190d110);
	writel(0xFFFFFFFF, 0x0190d114);
	writel(0xFFFFFFFF, 0x0190d120);
	writel(0xFFFFFFFF, 0x0190d124);
	/* Delay for somewhile to wait reset de-assertion to be stable. */
	udelay(10000);
#endif
	/*
	 * TODO:
	 * PLL/Clock/DIV and reset for SYS/CPU/DDR/UART/SDIO/TIMER to work as expected in SPL.
	 */

	/* Pinumux/GPIO/Pad init. */

	/*
	 * TODO:
	 * UART/SDIO/(JTAG).
	 */

	/* UART/Console init. SPL should be capable to output messages on UART console from now on. */
	/* NOTE: timer_init will be called inside preloader_console_init(). */
	preloader_console_init();
	/* From now on, we have timer. */
#ifdef DEBUG /* TO-BE-CLEAN: debug purpose, remove it in cleanup stage. Jqiu3, 2018/06/11. */
	puts("[SPL_BOARD_INIT] DELAY TEST. DELAY 5 sec!\n");
	udelay(5*1000*1000);
	puts("[SPL_BOARD_INIT] DELAY TEST END.\n");
#endif

	/* SDRAM part. */
	/*ddr_init();*/
}

int board_mmc_init(bd_t *dis)
{
	spl_mmc_init();
	return 0;
}
