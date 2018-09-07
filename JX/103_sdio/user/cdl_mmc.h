/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_MMC_H__
#define __CDL_MMC_H__

/* Register offsets */
#define DWMCI_CTRL	0x000
#define DWMCI_PWREN 	0x004
#define DWMCI_CLKDIV	0x008
#define DWMCI_CLKSRC	0x00C
#define DWMCI_CLKENA	0x010
#define DWMCI_TMOUT 	0x014
#define DWMCI_CTYPE 	0x018
#define DWMCI_BLKSIZ	0x01C
#define DWMCI_BYTCNT	0x020
#define DWMCI_INTMASK 	0x024
#define DWMCI_CMDARG	0x028
#define DWMCI_CMD 	0x02C
#define DWMCI_RESP0 	0x030
#define DWMCI_RESP1 	0x034
#define DWMCI_RESP2 	0x038
#define DWMCI_RESP3 	0x03C
#define DWMCI_MINTSTS 	0x040
#define DWMCI_RINTSTS 	0x044
#define DWMCI_STATUS	0x048
#define DWMCI_FIFOTH	0x04C
#define DWMCI_CDETECT 	0x050
#define DWMCI_WRTPRT	0x054
#define DWMCI_GPIO	0x058
#define DWMCI_TCMCNT	0x05C
#define DWMCI_TBBCNT	0x060
#define DWMCI_DEBNCE	0x064
#define DWMCI_USRID 	0x068
#define DWMCI_VERID 	0x06C
#define DWMCI_HCON	0x070
#define DWMCI_UHS_REG 	0x074
#define DWMCI_RST_CARD 	0x078
#define DWMCI_BMOD	0x080
#define DWMCI_PLDMND	0x084
#define DWMCI_DBADDR	0x088
#define DWMCI_IDSTS 	0x08C
#define DWMCI_IDINTEN 	0x090
#define DWMCI_DSCADDR 	0x094
#define DWMCI_BUFADDR 	0x098
#define DWMCI_CDTHRCTL 	0x100
#define DWMCI_DATA	0x200

/* Interrupt Mask register 0x024 */
#define DWMCI_INTMSK_ALL	0xffffffff
#define DWMCI_INTMSK_CD 	(1 << 0)
#define DWMCI_INTMSK_RE 	(1 << 1)
#define DWMCI_INTMSK_CDONE	(1 << 2)
#define DWMCI_INTMSK_DTO	(1 << 3)
#define DWMCI_INTMSK_TXDR 	(1 << 4)
#define DWMCI_INTMSK_RXDR 	(1 << 5)
#define DWMCI_INTMSK_DCRC 	(1 << 7)
#define DWMCI_INTMSK_RTO	(1 << 8)
#define DWMCI_INTMSK_DRTO 	(1 << 9)
#define DWMCI_INTMSK_HTO	(1 << 10)
#define DWMCI_INTMSK_FRUN 	(1 << 11)
#define DWMCI_INTMSK_HLE	(1 << 12)
#define DWMCI_INTMSK_SBE	(1 << 13)
#define DWMCI_INTMSK_ACD	(1 << 14)
#define DWMCI_INTMSK_EBE	(1 << 15)

/* Raw interrupt Regsiter 0X44*/
#define DWMCI_DATA_ERR	(DWMCI_INTMSK_EBE | DWMCI_INTMSK_SBE | DWMCI_INTMSK_HLE |\
			DWMCI_INTMSK_FRUN | DWMCI_INTMSK_EBE | DWMCI_INTMSK_DCRC)
#define DWMCI_DATA_TOUT (DWMCI_INTMSK_HTO | DWMCI_INTMSK_DRTO)

/* CTRL register 0x0*/
#define DWMCI_CTRL_RESET	(1 << 0)
#define DWMCI_CTRL_FIFO_RESET (1 << 1)
#define DWMCI_CTRL_DMA_RESET	(1 << 2)
#define DWMCI_CTRL_INT_EN (1 << 4)
#define DWMCI_DMA_EN		(1 << 5)
#define DWMCI_CTRL_SEND_AS_CCSD (1 << 10)
#define DWMCI_IDMAC_EN		(1 << 25)
#define DWMCI_RESET_ALL 	(DWMCI_CTRL_RESET | DWMCI_CTRL_FIFO_RESET |\
				DWMCI_CTRL_DMA_RESET)

/* CMD register 0x2c*/
#define DWMCI_CMD_RESP_EXP	(1 << 6)
#define DWMCI_CMD_RESP_LENGTH 	(1 << 7)
#define DWMCI_CMD_CHECK_CRC 	(1 << 8)
#define DWMCI_CMD_DATA_EXP	(1 << 9)
#define DWMCI_CMD_RW		(1 << 10)
#define DWMCI_CMD_SEND_STOP 	(1 << 12)
#define DWMCI_CMD_ABORT_STOP	(1 << 14)
#define DWMCI_CMD_PRV_DAT_WAIT	(1 << 13)
#define DWMCI_CMD_UPD_CLK 	(1 << 21)
#define DWMCI_CMD_USE_HOLD_REG	(1 << 29)
#define DWMCI_CMD_START 	(1 << 31)

/* CLKENA register 0x10 */
#define DWMCI_CLKEN_ENABLE	(1 << 0)
#define DWMCI_CLKEN_LOW_PWR 	(1 << 16)

/* Card-type registe 0x18*/
#define DWMCI_CTYPE_1BIT	0
#define DWMCI_CTYPE_4BIT	(1 << 0)
#define DWMCI_CTYPE_8BIT	(1 << 16)

/* Status Register 0x48*/
#define DWMCI_BUSY		(1 << 9)
#define DWMCI_FIFO_MASK 	0x1fff
#define DWMCI_FIFO_SHIFT	17
#define DWMCI_FIFO_EMPTY	(1<< 2)
#define DWMCI_FIFO_FULL  	(1<< 3)

/* FIFOTH Register 0x4c*/
#define MSIZE(x)	((x) << 28)
#define RX_WMARK(x) 	((x) << 16)
#define TX_WMARK(x) 	(x)
#define RX_WMARK_SHIFT	16
#define RX_WMARK_MASK 	(0xfff << RX_WMARK_SHIFT)

/* Bits in IDMAC DES0 Element (32-bit Address Configuration) */
#define DWMCI_IDMAC_OWN 	(1 << 31)
#define DWMCI_IDMAC_CH		(1 << 4)
#define DWMCI_IDMAC_FS		(1 << 3)
#define DWMCI_IDMAC_LD		(1 << 2)

/*	Bus Mode Register 0x80*/
#define DWMCI_BMOD_IDMAC_RESET	(1 << 0)
#define DWMCI_BMOD_IDMAC_FB 	(1 << 1)
#define DWMCI_BMOD_IDMAC_EN 	(1 << 7)

/* UHS register 0x74*/
#define DWMCI_DDR_MODE	(1 << 16)

/* quirks */
#define DWMCI_QUIRK_DISABLE_SMU 	(1 << 0)

#define DWMMC_MAX_FREQ			52000000
#define DWMMC_MIN_FREQ			400000
#define CONFIG_SYS_MMC_MAX_BLK_COUNT	256

/* MMC relative data structure*/
#define MMC_DATA_READ 	1
#define MMC_DATA_WRITE	2

#define NO_CARD_ERR 	-16 /* No SD/MMC card inserted */
#define UNUSABLE_ERR	-17 /* Unusable Card */
#define COMM_ERR	-18 /* Communications Error */
#define TIMEOUT 	-19
#define SWITCH_ERR	-20 /* Card reports failure to switch mode */

#define MMC_CMD_STOP_TRANSMISSION 12

#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136 	(1 << 1)	/* 136 bit response */
#define MMC_RSP_CRC 	(1 << 2)	/* expect valid crc */
#define MMC_RSP_BUSY	(1 << 3)	/* card may send busy */
#define MMC_RSP_OPCODE	(1 << 4)	/* response contains opcode */

#define MMC_VDD_165_195 0x00000080	/* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21 	0x00000100	/* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22 	0x00000200	/* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23 	0x00000400	/* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24 	0x00000800	/* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25 	0x00001000	/* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26 	0x00002000	/* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27 	0x00004000	/* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28 	0x00008000	/* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29 	0x00010000	/* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30 	0x00020000	/* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31 	0x00040000	/* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32 	0x00080000	/* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33 	0x00100000	/* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34 	0x00200000	/* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35 	0x00400000	/* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36 	0x00800000	/* VDD voltage 3.5 ~ 3.6 */

#define MMC_MODE_HS 		(1 << 0)
#define MMC_MODE_HS_52MHz 	(1 << 1)
#define MMC_MODE_4BIT 		(1 << 2)
#define MMC_MODE_8BIT 		(1 << 3)
#define MMC_MODE_SPI		(1 << 4)
#define MMC_MODE_DDR_52MHz	(1 << 5)

struct dw_mmc_cmd {
	unsigned short cmdidx;
	unsigned int resp_type;
	unsigned int cmdarg;
	unsigned int response[4];
};

struct dw_mmc_data {
	union {
		char* dest;
		const char* src; /* src buffers don't get written to */
	}addr;
	unsigned int flags;
	unsigned int blocks;
	unsigned int blocksize;
};

struct dwmci_host;
struct dw_mmc_ops {
	int (*send_cmd)(struct dwmci_host* host,
	        struct dw_mmc_cmd* cmd,
	        struct dw_mmc_data* data);
	void (*set_ios)(struct dwmci_host* host);
	int (*init)(struct dwmci_host* host);
	int (*getcd)(struct dwmci_host* host);
	int (*getwp)(struct dwmci_host* host);
};

struct dw_mmc_config {
	const struct dw_mmc_ops* ops;
	unsigned int host_caps;
	unsigned int voltages;
	unsigned int f_min;
	unsigned int f_max;
	unsigned int b_max;
};

struct dwmci_host {
	void* regs;
	unsigned int pclk;  /*input clock freq*/
	unsigned int clock;	 /*mmc config clock freq*/
	unsigned int div;
	unsigned int actual_clock;/* mmc actual clock freq*/
	unsigned int bus_width;
	unsigned long fifoth_val;
	unsigned int caps;

	int ddr_mode;

	int fifo_mode; /*fifo mode: 1  polling don't use idma ; 0 use idma*/

	struct dw_mmc_config cfg;

	int int_vector;
};

/*dma relative*/

/*DMA page size*/
#define PAGE_SIZE 4096
#define MAX_BLOCK_PER_TRANSFER		(64)
#define IDMAC_DESC_MAX_SIZE	(sizeof(struct dwmci_idmac)*MAX_BLOCK_PER_TRANSFER/8/4)

#define ARCH_DMA_MINALIGN 64
//struct dwmci_idmac {
//	unsigned long flags;
//	unsigned long cnt;
//	unsigned long addr;
//	unsigned long next_addr;
//} __attribute__((aligned(ARCH_DMA_MINALIGN)));

struct dwmci_idmac {
	unsigned long flags;
	unsigned long cnt;
	unsigned long addr;
	unsigned long next_addr;
};

/*apis*/
int dw_mmc_init(struct dwmci_host* host, unsigned long base, unsigned long pclk,
	int bus_width,int speed);

unsigned long dwmci_get_raw_int_status(struct dwmci_host* host);
unsigned long dwmci_get_int_status(struct dwmci_host* host);
void dwmci_clear_int_status(struct dwmci_host* host, unsigned long status);
void dwmci_set_int_mask(struct dwmci_host* host, unsigned long mask);
void dwmci_enable_int(struct dwmci_host* host,int en);

void dwmci_display_config(struct dwmci_host* host);
#endif
