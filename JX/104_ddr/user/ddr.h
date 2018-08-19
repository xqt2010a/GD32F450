#ifndef __DDRC_H__
#define __DDRC_H__

#define reg32_write(addr, val)  ((*((volatile unsigned int *) (addr))) = (val))
#define reg32_read(addr)        (*((volatile unsigned int *) (addr)))
#define reg16_write(addr, val)  ((*((volatile unsigned short *) (addr))) = (val))
#define reg16_read(addr)        (*((volatile unsigned short *) (addr)))

#define DDR_PHY_BASE_ADDR       0x04000000
#define DDRC_BASE_ADDR          0x05000000
#define DDRC_FREQ1_BASE         (DDRC_BASE_ADDR + 0x2000)
#define DDRC_FREQ2_BASE         (DDRC_BASE_ADDR + 0x3000)
#define DDRC_FREQ3_BASE         (DDRC_BASE_ADDR + 0x4000)

#define DDRC_BASE_REG(offset)   (DDRC_BASE_ADDR + offset)
#define DDRC_FREQ1_REG(offset)  (DDRC_FREQ1_BASE + offset)
#define DDRC_FREQ2_REG(offset)  (DDRC_FREQ2_BASE + offset)
#define DDRC_FREQ3_REG(offset)  (DDRC_FREQ3_BASE + offset)

#define DDRPHY_REG(offset)      (DDR_PHY_BASE_ADDR + 4 * offset)

#define IMEM_LEN                32768          //23400	//byte
#define DMEM_LEN                16384          //1720	//byte
#define IMEM_2D_OFFSET 	        49152

#define IMEM_OFFSET_ADDR        0x00050000
#define DMEM_OFFSET_ADDR        0x00054000

typedef enum  {
	FW_1D_IMAGE=0,
	FW_2D_IMAGE,
}fw_type;

void ddr_init(void);

#endif
