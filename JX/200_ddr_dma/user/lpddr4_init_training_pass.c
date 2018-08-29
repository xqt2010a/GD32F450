#include "ddr.h"
#include "lpddr4_init_training_pass.h"
#include "lpddr4_pmu_train_dmem.h"
#include "lpddr4_pmu_train_imem.h"
#include "lpddr4_2d_pmu_train_dmem.h"
#include "lpddr4_2d_pmu_train_imem.h"
//#include "data_type.h"
//#include "uart.h"

//#define BOOT_FROM_FREQ0    //3200M
//#define BOOT_FROM_FREQ1    //800M
//#define BOOT_FROM_FREQ2    //1600M
//#define BOOT_FROM_FREQ3    //2400M

#define SPL_LOADIMAGE_BUF_ADDR (0x1040000)

#define TRAIN_1D	(1)
#define TRAIN_2D	(2)



//static void reg32_write(unsigned long addr, unsigned long val)  
//{
//	REG32(addr) = val;
//}
//
//static unsigned long reg32_read(unsigned long addr)
//{
//	return REG32(addr);
//}        
//
//
static void ddrphy_apb_wr(unsigned long addr, unsigned long val)
{
	reg32_write(addr, val);
}


static int load_train_code(u32 dest, u32 size, u32* src_addr)
{
	u32 *imagebuf;
	u32 tmpidx = 0;
	//u32 buflen = SPL_LOADIMAGE_BUF_LEN; /*SHOULD BE 512B aligned*/
	//imagebuf = (u32*) SPL_LOADIMAGE_BUF_ADDR;
    imagebuf = src_addr;

	u32 i,tmplen, tmp32, pr_to32;
	
	tmplen = size;

#define ARC_RAM_ACCESS_STEPLEN	4
			pr_to32 =  dest + tmpidx*ARC_RAM_ACCESS_STEPLEN; 
			for(i = 0x0; i < tmplen/4; i++){
				tmp32 = imagebuf[i];
				reg16_write(pr_to32, tmp32 & 0x0000ffff);
				pr_to32 += ARC_RAM_ACCESS_STEPLEN; 
				reg16_write(pr_to32, (tmp32 >> 16) & 0x0000ffff);
				pr_to32 += ARC_RAM_ACCESS_STEPLEN; 
			}			

		if(1){
			// if(loadmode == 1) {
				pr_to32 =  dest + tmpidx*ARC_RAM_ACCESS_STEPLEN; 
				for(i = 0x0; i < tmplen/4;i++){
					tmp32 = (reg16_read(pr_to32) & 0x0000ffff);
					pr_to32 += ARC_RAM_ACCESS_STEPLEN;
					tmp32 += ((reg16_read(pr_to32) & 0x0000ffff) << 16);
			
					if(tmp32 != imagebuf[i]){
						debug("SPL: image verify failed.\n");
						return -1;
					}
					pr_to32 += ARC_RAM_ACCESS_STEPLEN;
				}
				debug("verify success\n");
			// }else {
			// 	for(i=0;i<tmplen/4;i++) {
			// 		if(imagebuf[i]!=((u32*)dest)[i]){
			// 			debug("SPL: image verify failed.\n");
			// 			return -1;
			// 		}
			// 	}
			// }
		}
		return 0;

}

int ddr_load_train_code_jtag(u8 num)
{
	
	u32 dest, isize, dsize;
	int err;
	
	if(num == TRAIN_1D)
	{
		isize = 31000;
		dsize = 1624;
	}
	else
	{
		isize = 23092;
		dsize = 1332;
	}

// imem
	dest = 0x4140000;
	

	err = load_train_code(dest, isize, (u32 *)D1_Imem);
	if(err)
	{
		debug("err num %d\n",err);
	}
	

//dmem
	dest = 0x4150000;
	

	err = load_train_code(dest, dsize, (u32 *)D1_Dmem);
	if(err)
	{
		debug("err num %d\n",err);
	}

	return 0;
}


#if 0 /* Move implemenation to SPL codes. */
unsigned long get_fireware()
{
	unsigned long addr_start = 0;
	/* Load the firewares to continuous memory regions, by the order IMEM_1D, DMEM_1D, IMEM_2D, DMEM_2D */
	/* Then return the start address of the momory region */
	return addr_start;
}

int ddr_load_train_code(fw_type type)
{
	int ret = 0;
	unsigned int tmp32, i;
	unsigned int error = 0;
	unsigned long pr_to32, pr_from32;
	unsigned long fw_offset = type ? IMEM_2D_OFFSET : 0;
	unsigned long imem_start = get_fireware() + fw_offset;
	unsigned long dmem_start = imem_start + IMEM_LEN;

	pr_from32 = imem_start;
	pr_to32 = DDRPHY_REG(IMEM_OFFSET_ADDR);
	for(i = 0x0; i < IMEM_LEN; ){
		tmp32 = reg32_read(pr_from32);
		reg16_write(pr_to32, tmp32 & 0x0000ffff);
		pr_to32 += 4;
		reg16_write(pr_to32, (tmp32 >> 16) & 0x0000ffff);
		pr_to32 += 4;
		pr_from32 += 4;
		i += 4;
	}

	pr_from32 = dmem_start;
	pr_to32 = DDRPHY_REG(DMEM_OFFSET_ADDR);
	for(i = 0x0; i < DMEM_LEN;){
		tmp32 = reg32_read(pr_from32);
		reg16_write(pr_to32, tmp32 & 0x0000ffff);
		pr_to32 += 4;
		reg16_write(pr_to32, (tmp32 >> 16) & 0x0000ffff);
		pr_to32 += 4;
		pr_from32 += 4;
		i += 4;
	}

	pr_from32 = imem_start;
	pr_to32 = DDRPHY_REG(IMEM_OFFSET_ADDR);
	for(i = 0x0; i < IMEM_LEN;){
		tmp32 = (reg16_read(pr_to32) & 0x0000ffff);
		pr_to32 += 4;
		tmp32 += ((reg16_read(pr_to32) & 0x0000ffff) << 16);

		if(tmp32 != reg32_read(pr_from32)){
			//printf("%lx %lx\n", pr_from32, pr_to32);
			error++;
		}
		pr_from32 += 4;
		pr_to32 += 4;
		i += 4;
	}
	if(error){
		//printf("check lpddr4_pmu_train_imem code fail=%d\n",error);
		ret = 1;
	}

	pr_from32 = dmem_start;
	pr_to32 = DDRPHY_REG(DMEM_OFFSET_ADDR);
	for(i = 0x0; i < DMEM_LEN;){
		tmp32 = (reg16_read(pr_to32) & 0x0000ffff);
		pr_to32 += 4;
		tmp32 += ((reg16_read(pr_to32) & 0x0000ffff) << 16);
		if(tmp32 != reg32_read(pr_from32)){
			//printf("%lx %lx\n", pr_from32, pr_to32);
			error++;
		}
		pr_from32 += 4;
		pr_to32 += 4;
		i += 4;
	}

	if(error){
		//printf("check lpddr4_pmu_train_dmem code fail=%d",error);
		ret = 1;
	}
	return ret;
}
#endif
static inline void poll_pmu_message_ready(void)
{
	unsigned int reg;
	do {
		reg = reg32_read(DDRPHY_REG(0xd0004));           
	} while (reg & 0x1);
}

static inline void ack_pmu_message_recieve(void)
{
	unsigned int reg;
	reg32_write(DDRPHY_REG(0xd0031), 0x0);             
	do {
		reg = reg32_read(DDRPHY_REG(0xd0004));           
	} while (!(reg & 0x1));
	reg32_write(DDRPHY_REG(0xd0031), 0x1);             
}

static inline unsigned int get_mail(void)
{
	unsigned int reg;
	poll_pmu_message_ready();
	reg = reg32_read(DDRPHY_REG(0xd0032));             
	ack_pmu_message_recieve();
	return reg;
}

static inline unsigned int get_stream_message(void)
{
	unsigned int reg, reg2;

	poll_pmu_message_ready();
	reg = reg32_read(DDRPHY_REG(0xd0032));            
	reg2 = reg32_read(DDRPHY_REG(0xd0034));           
	reg2 = (reg2 << 16) | reg;
	ack_pmu_message_recieve();

	return reg2;
}

static inline void decode_major_message(unsigned int mail)
{
	debug("[PMU Major message = 0x%08x]\n", mail);
	return;
}

static inline void decode_streaming_message(void)
{
	unsigned int string_index, arg;
	int i = 0;

	string_index = get_stream_message();
	debug("	PMU String index = 0x%08x\n", string_index);
	while (i < (string_index & 0xffff)){
		arg = get_stream_message();
		arg += 0;
		debug("	arg[%d] = 0x%08x\n", i, arg);
		i++;
	}

	debug("\n");
}

static int wait_ddrphy_training_complete(void)
{
	unsigned int mail;
	int ret = 0;
	while (1) {
		mail = get_mail();
		decode_major_message(mail);
		if (mail == 0x08) {
			decode_streaming_message();
		} else if (mail == 0x07) {
			debug("Training PASS\n");
            asm("nop");
            mail = mail;
			break;
		} else if (mail == 0xff) {
			debug("Training FAILED\n");
			ret = 1;
			break;
		}
	}
	return ret;
}


void set_ddrc_freq(u32 freq_MHZ)
{
	reg32_write(0x0190d02c, reg32_read(0x0190d02c) & 0x7FFFFFFF);
	reg32_write(0x0190d068, reg32_read(0x0190d068) | 0x80008000);
	reg32_write(0x0190d000, (reg32_read(0x0190d000) & 0xFFFFFF00) | 0xF0);
	switch(freq_MHZ)
	{
		/* Set the PHY input clocks back to pstate 0 -- 800MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
		case  3200 :
					reg32_write(0x0190d000, 0x0C1909f0);
					reg32_write(0x0190d000, 0x0C190900);
					break;
		/* Set the PHY input clocks back to pstate 0 -- 200MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
		case  800 :					
					reg32_write(0x0190d000, 0x0C0649f0);
					reg32_write(0x0190d000, 0x0C064900);
					break;
		case  1600 :
		/* Set the PHY input clocks back to pstate 0 -- 400MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
					reg32_write(0x0190d000, 0x0C0C89f0);
					reg32_write(0x0190d000, 0x0C0C8900);
					break;
		case  2400 :
		/* Set the PHY input clocks back to pstate 0 -- 600MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
					reg32_write(0x0190d000, 0x0C12C9f0);
					reg32_write(0x0190d000, 0x0C12C900);
					break;
		default :
					reg32_write(0x0190d000, 0x0C0649f0);
					reg32_write(0x0190d000, 0x0C064900);
	}
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000);
	reg32_write(0x0190d02c, reg32_read(0x0190d02c) | 0x80000000);

	return;
}

//void dram_controller_cfg(void)
//{
//	reg32_write(DDRC_BASE_REG(0x304), 0x00000001);           
//	reg32_write(DDRC_BASE_REG(0x30), 0x00000001);            
//	reg32_write(DDRC_BASE_REG(0x0), 0xA1080020);             // Use 1 rank other than 2, as CS1 is not connected
//	/* Freq0 settings (800M data rate) */
//	reg32_write(DDRC_BASE_REG(0x28), 0x00000000);            
//	reg32_write(DDRC_BASE_REG(0x20), 0x00000001);           // DERATEEN
//	reg32_write(DDRC_BASE_REG(0x24), 0x0061A800);           // DERATEINT
//	reg32_write(DDRC_BASE_REG(0x50), 0x0020D040);           // RFSHCTL0
//	reg32_write(DDRC_BASE_REG(0x64), 0x00180024);           // RFSHTMG
//	reg32_write(DDRC_BASE_REG(0xD0), 0xC0020188); 
//	reg32_write(DDRC_BASE_REG(0xD4), 0x009D0000);            
//	reg32_write(DDRC_BASE_REG(0xDC), 0x00940009);           // INIT3
//	reg32_write(DDRC_BASE_REG(0xE0), 0x00310000);           // INIT4 
//	reg32_write(DDRC_BASE_REG(0xE8), 0x0055004A);           // INIT6
//	reg32_write(DDRC_BASE_REG(0xEC), 0x0015004A);           // INIT7 
//	
//	reg32_write(DDRC_BASE_REG(0x100), 0x0C080609);         // DRAMTMG0  
//	reg32_write(DDRC_BASE_REG(0x104), 0x0003040D);         // DRAMTMG1   
//	reg32_write(DDRC_BASE_REG(0x108), 0x0305090C);         // DRAMTMG2     
//	reg32_write(DDRC_BASE_REG(0x10C), 0x00505000);         // DRAMTMG3
//	reg32_write(DDRC_BASE_REG(0x110), 0x04040204);         // DRAMTMG4
//	reg32_write(DDRC_BASE_REG(0x114), 0x02030303);         // DRAMTMG5 
//	reg32_write(DDRC_BASE_REG(0x118), 0x01010004);         // DRAMTMG6
//	reg32_write(DDRC_BASE_REG(0x11C), 0x00000301);         // DRAMTMG7 
//	reg32_write(DDRC_BASE_REG(0x130), 0x00020300);         // DRAMTMG12
//	reg32_write(DDRC_BASE_REG(0x134), 0x0A100002);         // DRAMTMG13
//	reg32_write(DDRC_BASE_REG(0x138), 0x00000026);         // DRAMTMG14 
//	reg32_write(DDRC_BASE_REG(0x144), 0x00280014);         // DRAMTMG17   
//
//	reg32_write(DDRC_BASE_REG(0x180), 0x00C80006);          // ZQCTL0 
//	reg32_write(DDRC_BASE_REG(0x184), 0x00A0186A);           
//	reg32_write(DDRC_BASE_REG(0x188), 0x00000000);
//	
//	reg32_write(DDRC_BASE_REG(0x190), 0x03858202);          // DFITMG0 
//	reg32_write(DDRC_BASE_REG(0x194), 0x00080303);          // DFITMG1   
//	reg32_write(DDRC_BASE_REG(0x1A0), 0xE0400018);           
//	reg32_write(DDRC_BASE_REG(0x1A4), 0x00DF00E4);           
//	reg32_write(DDRC_BASE_REG(0x1A8), 0x80000000);           
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);           
//	reg32_write(DDRC_BASE_REG(0x1B4), 0x00000502);          // DFITMG2 
//	reg32_write(DDRC_BASE_REG(0x1C0), 0x00000001);           
//	reg32_write(DDRC_BASE_REG(0x1C4), 0x00000001);	         
//	
//	reg32_write(DDRC_BASE_REG(0xF4), 0x00000639);            
//	reg32_write(DDRC_BASE_REG(0x200), 0x0000001f);          // Use 1 rank other than 2  
//	reg32_write(DDRC_BASE_REG(0x208), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x20C), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x210), 0x00001F1F);           
//	reg32_write(DDRC_BASE_REG(0x204), 0x00080808);           
//	reg32_write(DDRC_BASE_REG(0x214), 0x07070707);           
//	reg32_write(DDRC_BASE_REG(0x218), 0x0f070707);         // Use 1 rank other than 2    
//	reg32_write(DDRC_BASE_REG(0x21C), 0x0000000F);           
//	reg32_write(DDRC_BASE_REG(0x224), 0x07070707);           
//	reg32_write(DDRC_BASE_REG(0x228), 0x07070707);                                                                                                                   
//
//	reg32_write(DDRC_BASE_REG(0x244), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x250), 0x29001505);           
//	reg32_write(DDRC_BASE_REG(0x254), 0x0000002C);           
//	reg32_write(DDRC_BASE_REG(0x25C), 0x5900575B);           
//	reg32_write(DDRC_BASE_REG(0x264), 0x00000009);           
//	reg32_write(DDRC_BASE_REG(0x26C), 0x02005574);           
//
//	reg32_write(DDRC_BASE_REG(0x300), 0x00000016);           
//	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x30C), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
//	
//	reg32_write(DDRC_BASE_REG(0x36C), 0x00000011);           
//	reg32_write(DDRC_BASE_REG(0x400), 0x00000111);           
//	reg32_write(DDRC_BASE_REG(0x404), 0x000010F3);           
//	reg32_write(DDRC_BASE_REG(0x408), 0x000072FF);		       
//	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);           
//	reg32_write(DDRC_BASE_REG(0x494), 0x01110D00);           
//	reg32_write(DDRC_BASE_REG(0x498), 0x00620790);           
//	reg32_write(DDRC_BASE_REG(0x49C), 0x00100001);           
//	reg32_write(DDRC_BASE_REG(0x4A0), 0x0000041F);           
//
//	return;
//}

void dram_phy_cfg_train(void)
{
	//ddrphy_apb_wr(DDRPHY_REG(0x20110), 0x02);              
	//ddrphy_apb_wr(DDRPHY_REG(0x20111), 0x03);              
	//ddrphy_apb_wr(DDRPHY_REG(0x20112), 0x04);              
	//ddrphy_apb_wr(DDRPHY_REG(0x20113), 0x05);              
	//ddrphy_apb_wr(DDRPHY_REG(0x20114), 0x00);              
	//ddrphy_apb_wr(DDRPHY_REG(0x20115), 0x01);              

	/* Initialize PHY Configuration */
	ddrphy_apb_wr(DDRPHY_REG(0x1005f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1015f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1105f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1115f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1205f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1215f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1305f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1315f), 0x1ff);               

	ddrphy_apb_wr(DDRPHY_REG(0x55), 0x1ff);                  
	ddrphy_apb_wr(DDRPHY_REG(0x1055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x2055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x3055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x4055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x5055), 0x1ff);                
	//ddrphy_apb_wr(DDRPHY_REG(0x6055), 0x1ff);              
	//ddrphy_apb_wr(DDRPHY_REG(0x7055), 0x1ff);              
	//ddrphy_apb_wr(DDRPHY_REG(0x8055), 0x1ff);              
	//ddrphy_apb_wr(DDRPHY_REG(0x9055), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x200c5), 0x7); 		//pll freq
	ddrphy_apb_wr(DDRPHY_REG(0x200c7), 0x100);			//TYRION  200MHZ



	ddrphy_apb_wr(DDRPHY_REG(0x2002e), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x90204), 0x0);                 

	ddrphy_apb_wr(DDRPHY_REG(0x20024), 0xa3);                
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20056), 0xa);                 

	ddrphy_apb_wr(DDRPHY_REG(0x1004d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1014d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1104d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1114d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1204d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1214d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1304d), 0x1a);               
	ddrphy_apb_wr(DDRPHY_REG(0x1314d), 0x1a);                  

	ddrphy_apb_wr(DDRPHY_REG(0x10049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x10149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x11049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x11149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x12049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x12149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x13049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x13149), 0x69a);               
	
	ddrphy_apb_wr(DDRPHY_REG(0x43), 0x63);                   
	ddrphy_apb_wr(DDRPHY_REG(0x1043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x2043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x3043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x4043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x6043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x7043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x8043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x9043), 0x63);                 

	ddrphy_apb_wr(DDRPHY_REG(0x20018), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20075), 0x4);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20050), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20008), 0xc8);               
	ddrphy_apb_wr(DDRPHY_REG(0x20088), 0x9);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200b2), 0x107);  		//tyrion  : 0x104   //labc
	ddrphy_apb_wr(DDRPHY_REG(0x10043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x10143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13143), 0x5a1);               
  
	ddrphy_apb_wr(DDRPHY_REG(0x200fa), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20019), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f0), 0x5550);
	ddrphy_apb_wr(DDRPHY_REG(0x200f1), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f2), 0x4444);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f3), 0x8888);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f4), 0x5655);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f5), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f6), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f7), 0xf000);              
	ddrphy_apb_wr(DDRPHY_REG(0x2000b), 0x19);                
	ddrphy_apb_wr(DDRPHY_REG(0x2000c), 0x32);                
	ddrphy_apb_wr(DDRPHY_REG(0x2000d), 0x1f4);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000e), 0x10);              

	ddrphy_apb_wr(DDRPHY_REG(0x20025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x2002d), 0x0);                      

	 /* Load the 1D IMEM image */
	 ddrphy_apb_wr(DDRPHY_REG(0x20060), 0x2);               
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	 // ddr_load_train_code(FW_1D_IMAGE);                        
	 ddr_load_train_code_jtag(TRAIN_1D);
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	 /* Ignore PHY input clocks setting for pstate 0 -- 200MHz, as there is only 1 pstate. */

	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                              
	 ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x320);               
	 ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x3C3C);       // Need double check for PhyOdtImpedance & PhyDrvImpedance in SI simulation        
	 ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x15);           // Need to double check the vref value =   vRef*128/vDDQ
	 ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);             
	 ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x131f);         //0x131F training all     
	 ddrphy_apb_wr(DDRPHY_REG(0x54009), 0x05);           //  Detailed debug messages.             
	 ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x120);        //  Single CS other than dual CS       
	 ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x0994);    	// Byte offset 0x32, CSR Addr 0x54019, Direction=In
                               						// Value to be programmed in DRAM Mode Register 1 {Channel A, Rank 0}          
	 ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a55);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a00);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x15);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x0994);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a55);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a00);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x15);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x0);          //  Single CS other than dual CS             
	 ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x9400);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3109);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x5500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x004a);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x1500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x9400);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3109);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x5500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x004a);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1500);                          
	 ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54044), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x9);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x0);                 

	 /* wait for train complete */
	 wait_ddrphy_training_complete();

	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	 /* Load the 2D IMEM image */
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	 //ddr_load_train_code(FW_2D_IMAGE);  
	 ddr_load_train_code_jtag(TRAIN_2D);                      
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	 /* 800 mts 2D training */
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x320);               
	 ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x3232);         // Need double check for PhyOdtImpedance & PhyDrvImpedance in SI simulation       
	 ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x14);             // Need to double check the vref value =   vRef*128/vDDQ   
	 ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x61);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54009), 0x05);              //  Detailed debug messages.               
	 ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x100);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x100);               
	 ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x8020);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x120);               
	 ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x0994);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a55);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a00);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x15);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x0994);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a55);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a00);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x15);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x0);         //  Single CS other than dual CS                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x9400);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3109);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x5500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x004a);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	 ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x1500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x9400);              
	 ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3109);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x5500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x004a);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	 ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1500);              
	 ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0x54044), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x9);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x0);                 

	 /* wait for train complete */
	 wait_ddrphy_training_complete();

	 ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	 ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                   

    //set_ddrc_freq();

	/* (I) Load PHY Init Engine Image */
#if 0
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                
	ddrphy_apb_wr(DDRPHY_REG(0x90000), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90001), 0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x90002), 0x10e);
	ddrphy_apb_wr(DDRPHY_REG(0x90003), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90004), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90005), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90029), 0xb);
	ddrphy_apb_wr(DDRPHY_REG(0x9002a), 0x480);
	ddrphy_apb_wr(DDRPHY_REG(0x9002b), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9002c), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9002d), 0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x9002e), 0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x9002f), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90030), 0x478);
	ddrphy_apb_wr(DDRPHY_REG(0x90031), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90032), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90033), 0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x90034), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90035), 0x2);
	ddrphy_apb_wr(DDRPHY_REG(0x90036), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90037), 0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x90038), 0xb);
	ddrphy_apb_wr(DDRPHY_REG(0x90039), 0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9003a), 0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x9003b), 0x44);
	ddrphy_apb_wr(DDRPHY_REG(0x9003c), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9003d), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9003e), 0x14f);
	ddrphy_apb_wr(DDRPHY_REG(0x9003f), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90040), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90041), 0x47);
	ddrphy_apb_wr(DDRPHY_REG(0x90042), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90043), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90044), 0x4f);
	ddrphy_apb_wr(DDRPHY_REG(0x90045), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90046), 0x179);
	ddrphy_apb_wr(DDRPHY_REG(0x90047), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90048), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x90049), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9004a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9004b), 0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x9004c), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9004d), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9004e), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x9004f), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90050), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90051), 0x45a);
	ddrphy_apb_wr(DDRPHY_REG(0x90052), 0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x90053), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90054), 0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x90055), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90056), 0x40);
	ddrphy_apb_wr(DDRPHY_REG(0x90057), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90058), 0x179);
	ddrphy_apb_wr(DDRPHY_REG(0x90059), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x9005a), 0x618);
	ddrphy_apb_wr(DDRPHY_REG(0x9005b), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9005c), 0x40c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9005d), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9005e), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9005f), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90060), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90061), 0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90062), 0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x90063), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90064), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90065), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90066), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90067), 0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90068), 0x40);
	ddrphy_apb_wr(DDRPHY_REG(0x90069), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9006a), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9006b), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x9006c), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x9006d), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x9006e), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9006f), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90070), 0x78);
	ddrphy_apb_wr(DDRPHY_REG(0x90071), 0x549);
	ddrphy_apb_wr(DDRPHY_REG(0x90072), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90073), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90074), 0xd49);
	ddrphy_apb_wr(DDRPHY_REG(0x90075), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90076), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90077), 0x94a);
	ddrphy_apb_wr(DDRPHY_REG(0x90078), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90079), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9007a), 0x441);
	ddrphy_apb_wr(DDRPHY_REG(0x9007b), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9007c), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9007d), 0x42);
	ddrphy_apb_wr(DDRPHY_REG(0x9007e), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9007f), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90080), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x90081), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90082), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90083), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x90085), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90086), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x90087), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90088), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90089), 0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9008a), 0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9008b), 0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9008c), 0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9008d), 0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9008e), 0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9008f), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90090), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90091), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90092), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90093), 0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x90094), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90095), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90096), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90097), 0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90098), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90099), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x9009a), 0x58);
	ddrphy_apb_wr(DDRPHY_REG(0x9009b), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x9009c), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x9009d), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9009e), 0x2);
	ddrphy_apb_wr(DDRPHY_REG(0x9009f), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900a0), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900a1), 0x5);
	ddrphy_apb_wr(DDRPHY_REG(0x900a2), 0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a3), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900a4), 0xd);
	ddrphy_apb_wr(DDRPHY_REG(0x900a5), 0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a6), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900a7), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x900a8), 0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a9), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x40000), 0x811);
	ddrphy_apb_wr(DDRPHY_REG(0x40020), 0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x40040), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40060), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40001), 0x4008);
	ddrphy_apb_wr(DDRPHY_REG(0x40021), 0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40041), 0x4f);
	ddrphy_apb_wr(DDRPHY_REG(0x40061), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40002), 0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x40022), 0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40042), 0x51);
	ddrphy_apb_wr(DDRPHY_REG(0x40062), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40003), 0x811);
	ddrphy_apb_wr(DDRPHY_REG(0x40023), 0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x40043), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40063), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40004), 0x720);
	ddrphy_apb_wr(DDRPHY_REG(0x40024), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40044), 0x1740);
	ddrphy_apb_wr(DDRPHY_REG(0x40064), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40005), 0x16);
	ddrphy_apb_wr(DDRPHY_REG(0x40025), 0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40045), 0x4b);
	ddrphy_apb_wr(DDRPHY_REG(0x40065), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40006), 0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40026), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40046), 0x2001);
	ddrphy_apb_wr(DDRPHY_REG(0x40066), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40007), 0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40027), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40047), 0x2800);
	ddrphy_apb_wr(DDRPHY_REG(0x40067), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40008), 0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40028), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40048), 0xf00);
	ddrphy_apb_wr(DDRPHY_REG(0x40068), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40009), 0x720);
	ddrphy_apb_wr(DDRPHY_REG(0x40029), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40049), 0x1400);
	ddrphy_apb_wr(DDRPHY_REG(0x40069), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000a), 0xe08);
	ddrphy_apb_wr(DDRPHY_REG(0x4002a), 0xc15);
	ddrphy_apb_wr(DDRPHY_REG(0x4004a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000b), 0x623);
	ddrphy_apb_wr(DDRPHY_REG(0x4002b), 0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x4004b), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006b), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000c), 0x4028);
	ddrphy_apb_wr(DDRPHY_REG(0x4002c), 0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x4004c), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006c), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000d), 0xe08);
	ddrphy_apb_wr(DDRPHY_REG(0x4002d), 0xc1a);
	ddrphy_apb_wr(DDRPHY_REG(0x4004d), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006d), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000e), 0x623);
	ddrphy_apb_wr(DDRPHY_REG(0x4002e), 0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x4004e), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006e), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000f), 0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x4002f), 0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x4004f), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006f), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40010), 0x2604);
	ddrphy_apb_wr(DDRPHY_REG(0x40030), 0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40050), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40070), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40011), 0x708);
	ddrphy_apb_wr(DDRPHY_REG(0x40031), 0x5);
	ddrphy_apb_wr(DDRPHY_REG(0x40051), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40071), 0x2002);
	ddrphy_apb_wr(DDRPHY_REG(0x40012), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x40032), 0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x40052), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40072), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40013), 0x2604);
	ddrphy_apb_wr(DDRPHY_REG(0x40033), 0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40053), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40073), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40014), 0x708);
	ddrphy_apb_wr(DDRPHY_REG(0x40034), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x40054), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40074), 0x2002);
	ddrphy_apb_wr(DDRPHY_REG(0x40015), 0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x40035), 0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x40055), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40075), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40016), 0x60a);
	ddrphy_apb_wr(DDRPHY_REG(0x40036), 0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40056), 0x1200);
	ddrphy_apb_wr(DDRPHY_REG(0x40076), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40017), 0x61a);
	ddrphy_apb_wr(DDRPHY_REG(0x40037), 0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40057), 0x1300);
	ddrphy_apb_wr(DDRPHY_REG(0x40077), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40018), 0x60a);
	ddrphy_apb_wr(DDRPHY_REG(0x40038), 0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40058), 0x1200);
	ddrphy_apb_wr(DDRPHY_REG(0x40078), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40019), 0x642);
	ddrphy_apb_wr(DDRPHY_REG(0x40039), 0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40059), 0x1300);
	ddrphy_apb_wr(DDRPHY_REG(0x40079), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4001a), 0x4808);
	ddrphy_apb_wr(DDRPHY_REG(0x4003a), 0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x4005a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4007a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900aa), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900ab), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900ac), 0x11a);
	ddrphy_apb_wr(DDRPHY_REG(0x900ad), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900ae), 0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x900af), 0x2a);
	ddrphy_apb_wr(DDRPHY_REG(0x900b0), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900b1), 0x7b2);
	ddrphy_apb_wr(DDRPHY_REG(0x900b2), 0x2a);
	ddrphy_apb_wr(DDRPHY_REG(0x900b3), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900b4), 0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x900b5), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900b6), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900b7), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900b8), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900b9), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900ba), 0x2a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900bb), 0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x900bc), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900bd), 0x370);
	ddrphy_apb_wr(DDRPHY_REG(0x900be), 0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x900bf), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x900c0), 0x3c8);
	ddrphy_apb_wr(DDRPHY_REG(0x900c1), 0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x900c2), 0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x900c3), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900c4), 0x199);
	ddrphy_apb_wr(DDRPHY_REG(0x900c5), 0x14);
	ddrphy_apb_wr(DDRPHY_REG(0x900c6), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900c7), 0x11a);
	ddrphy_apb_wr(DDRPHY_REG(0x900c8), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900c9), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x900ca), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x900cb), 0xe);
	ddrphy_apb_wr(DDRPHY_REG(0x900cc), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900cd), 0x199);
	ddrphy_apb_wr(DDRPHY_REG(0x900ce), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900cf), 0x8568);
	ddrphy_apb_wr(DDRPHY_REG(0x900d0), 0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x900d1), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x900d2), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900d3), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900d4), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900d5), 0x1d8);
	ddrphy_apb_wr(DDRPHY_REG(0x900d6), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900d7), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900d8), 0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x900d9), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900da), 0x70);
	ddrphy_apb_wr(DDRPHY_REG(0x900db), 0x788);
	ddrphy_apb_wr(DDRPHY_REG(0x900dc), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900dd), 0x1ff8);
	ddrphy_apb_wr(DDRPHY_REG(0x900de), 0x85a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900df), 0x1e8);
	ddrphy_apb_wr(DDRPHY_REG(0x900e0), 0x50);
	ddrphy_apb_wr(DDRPHY_REG(0x900e1), 0x798);
	ddrphy_apb_wr(DDRPHY_REG(0x900e2), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900e3), 0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x900e4), 0x7a0);
	ddrphy_apb_wr(DDRPHY_REG(0x900e5), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900e6), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900e7), 0x8310);
	ddrphy_apb_wr(DDRPHY_REG(0x900e8), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900e9), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900ea), 0xa310);
	ddrphy_apb_wr(DDRPHY_REG(0x900eb), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900ec), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x900ed), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900ee), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900ef), 0x6e);
	ddrphy_apb_wr(DDRPHY_REG(0x900f0), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f1), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x900f2), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f3), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900f4), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900f5), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f6), 0x8310);
	ddrphy_apb_wr(DDRPHY_REG(0x900f7), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900f8), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f9), 0xa310);
	ddrphy_apb_wr(DDRPHY_REG(0x900fa), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900fb), 0x1ff8);
	ddrphy_apb_wr(DDRPHY_REG(0x900fc), 0x85a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900fd), 0x1e8);
	ddrphy_apb_wr(DDRPHY_REG(0x900fe), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x900ff), 0x798);
	ddrphy_apb_wr(DDRPHY_REG(0x90100), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90101), 0x78);
	ddrphy_apb_wr(DDRPHY_REG(0x90102), 0x7a0);
	ddrphy_apb_wr(DDRPHY_REG(0x90103), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90104), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90105), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x90106), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90107), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90108), 0x8b10);
	ddrphy_apb_wr(DDRPHY_REG(0x90109), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x9010a), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9010b), 0xab10);
	ddrphy_apb_wr(DDRPHY_REG(0x9010c), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x9010d), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x9010e), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x9010f), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90110), 0x58);
	ddrphy_apb_wr(DDRPHY_REG(0x90111), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90112), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90113), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90114), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90115), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90116), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90117), 0x8b10);
	ddrphy_apb_wr(DDRPHY_REG(0x90118), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90119), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9011a), 0xab10);
	ddrphy_apb_wr(DDRPHY_REG(0x9011b), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x9011c), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9011d), 0x1d8);
	ddrphy_apb_wr(DDRPHY_REG(0x9011e), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x9011f), 0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x90120), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x90121), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90122), 0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90123), 0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x90124), 0x6a);
	ddrphy_apb_wr(DDRPHY_REG(0x90125), 0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x90126), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90127), 0x1e9);
	ddrphy_apb_wr(DDRPHY_REG(0x90128), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90129), 0x8080);
	ddrphy_apb_wr(DDRPHY_REG(0x9012a), 0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x9012b), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x9012c), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x9012d), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x9012e), 0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x9012f), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90130), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90131), 0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x90132), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90133), 0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x90134), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90135), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90136), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90137), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90138), 0x8080);
	ddrphy_apb_wr(DDRPHY_REG(0x90139), 0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x9013a), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9013b), 0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x9013c), 0x6a);
	ddrphy_apb_wr(DDRPHY_REG(0x9013d), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9013e), 0x8568);
	ddrphy_apb_wr(DDRPHY_REG(0x9013f), 0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x90140), 0xb7);
	ddrphy_apb_wr(DDRPHY_REG(0x90141), 0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x90142), 0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90143), 0x1f);
	ddrphy_apb_wr(DDRPHY_REG(0x90144), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90145), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90146), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90147), 0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x90148), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90149), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x9014a), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x9014b), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x9014c), 0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x9014d), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9014e), 0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x9014f), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90150), 0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90151), 0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90152), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90153), 0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x90154), 0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90155), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90156), 0x3c8);
	ddrphy_apb_wr(DDRPHY_REG(0x90157), 0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x90158), 0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x90159), 0x370);
	ddrphy_apb_wr(DDRPHY_REG(0x9015a), 0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x9015b), 0x20);
	ddrphy_apb_wr(DDRPHY_REG(0x9015c), 0x2aa);
	ddrphy_apb_wr(DDRPHY_REG(0x9015d), 0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9015e), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9015f), 0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x90160), 0x10e);
	ddrphy_apb_wr(DDRPHY_REG(0x90161), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90162), 0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x90163), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90164), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90165), 0x8140);
	ddrphy_apb_wr(DDRPHY_REG(0x90166), 0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x90167), 0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90168), 0x8138);
	ddrphy_apb_wr(DDRPHY_REG(0x90169), 0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x9016a), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9016b), 0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x9016c), 0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x9016d), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9016e), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9016f), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90170), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90171), 0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x90172), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90173), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x90174), 0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x90175), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90176), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90177), 0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x90178), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90179), 0x47);
	ddrphy_apb_wr(DDRPHY_REG(0x9017a), 0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9017b), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9017c), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9017d), 0x618);
	ddrphy_apb_wr(DDRPHY_REG(0x9017e), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9017f), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90180), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x90181), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90182), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90183), 0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x90184), 0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90185), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90186), 0x8140);
	ddrphy_apb_wr(DDRPHY_REG(0x90187), 0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x90188), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90189), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x9018a), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9018b), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9018c), 0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x9018d), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9018e), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9018f), 0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x90190), 0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x90006), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90007), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90008), 0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90009), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000b), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0xd00e7), 0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x90017), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9001f), 0x2b);
	ddrphy_apb_wr(DDRPHY_REG(0x90026), 0x6c);
	ddrphy_apb_wr(DDRPHY_REG(0x400d0), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x400d1), 0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x400d2), 0x105);
	ddrphy_apb_wr(DDRPHY_REG(0x400d3), 0x107);
	ddrphy_apb_wr(DDRPHY_REG(0x400d4), 0x10f);
	ddrphy_apb_wr(DDRPHY_REG(0x400d5), 0x202);
	ddrphy_apb_wr(DDRPHY_REG(0x400d6), 0x20a);
	ddrphy_apb_wr(DDRPHY_REG(0x400d7), 0x20b);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x2);                       
	ddrphy_apb_wr(DDRPHY_REG(0x9000c), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000d), 0x173);
	ddrphy_apb_wr(DDRPHY_REG(0x9000e), 0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x9000f), 0x6110);
	ddrphy_apb_wr(DDRPHY_REG(0x90010), 0x2152);
	ddrphy_apb_wr(DDRPHY_REG(0x90011), 0xdfbd);
	ddrphy_apb_wr(DDRPHY_REG(0x90012), 0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x90013), 0x6152);
	ddrphy_apb_wr(DDRPHY_REG(0x20010), 0x5a);                 
	ddrphy_apb_wr(DDRPHY_REG(0x20011), 0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x40080), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40081), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40082), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40083), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40085), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x400fd), 0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x10011), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10012), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10013), 0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x10018), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10002), 0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x100b2), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x101b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x102b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x103b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x104b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x105b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x106b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x107b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x108b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11011), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11012), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11013), 0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x11018), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11002), 0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x110b2), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x111b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x112b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x113b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x114b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x115b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x116b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x117b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x118b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12011), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12012), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12013), 0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x12018), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12002), 0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x120b2), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x121b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x122b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x123b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x124b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x125b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x126b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x127b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x128b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13011), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13012), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13013), 0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x13018), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13002), 0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x130b2), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x131b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x132b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x133b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x134b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x135b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x136b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x137b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x138b4), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x2);  

	ddrphy_apb_wr(DDRPHY_REG(0x20089), 0x1);                  
	ddrphy_apb_wr(DDRPHY_REG(0x20088), 0x19);       
	ddrphy_apb_wr(DDRPHY_REG(0xc0080), 0x2);                  
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);     
#else
	ddrphy_apb_wr(DDRPHY_REG(0xd0000),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90000),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90001),0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x90002),0x10e);
	ddrphy_apb_wr(DDRPHY_REG(0x90003),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90004),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90005),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90029),0xb);
	ddrphy_apb_wr(DDRPHY_REG(0x9002a),0x480);
	ddrphy_apb_wr(DDRPHY_REG(0x9002b),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9002c),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9002d),0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x9002e),0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x9002f),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90030),0x478);
	ddrphy_apb_wr(DDRPHY_REG(0x90031),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90032),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90033),0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x90034),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90035),0x2);
	ddrphy_apb_wr(DDRPHY_REG(0x90036),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90037),0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x90038),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x90039),0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9003a),0x139);
	ddrphy_apb_wr(DDRPHY_REG(0x9003b),0x44);
	ddrphy_apb_wr(DDRPHY_REG(0x9003c),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9003d),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9003e),0x14f);
	ddrphy_apb_wr(DDRPHY_REG(0x9003f),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90040),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90041),0x47);
	ddrphy_apb_wr(DDRPHY_REG(0x90042),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90043),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90044),0x4f);
	ddrphy_apb_wr(DDRPHY_REG(0x90045),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90046),0x179);
	ddrphy_apb_wr(DDRPHY_REG(0x90047),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90048),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x90049),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9004a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9004b),0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x9004c),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9004d),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9004e),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x9004f),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90050),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90051),0x45a);
	ddrphy_apb_wr(DDRPHY_REG(0x90052),0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x90053),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90054),0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x90055),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90056),0x40);
	ddrphy_apb_wr(DDRPHY_REG(0x90057),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90058),0x179);
	ddrphy_apb_wr(DDRPHY_REG(0x90059),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x9005a),0x618);
	ddrphy_apb_wr(DDRPHY_REG(0x9005b),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9005c),0x40c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9005d),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9005e),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9005f),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90060),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90061),0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90062),0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x90063),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90064),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90065),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90066),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90067),0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90068),0x40);
	ddrphy_apb_wr(DDRPHY_REG(0x90069),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9006a),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9006b),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x9006c),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x9006d),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x9006e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9006f),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90070),0x78);
	ddrphy_apb_wr(DDRPHY_REG(0x90071),0x549);
	ddrphy_apb_wr(DDRPHY_REG(0x90072),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90073),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90074),0xd49);
	ddrphy_apb_wr(DDRPHY_REG(0x90075),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90076),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x90077),0x94a);
	ddrphy_apb_wr(DDRPHY_REG(0x90078),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90079),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9007a),0x441);
	ddrphy_apb_wr(DDRPHY_REG(0x9007b),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9007c),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9007d),0x42);
	ddrphy_apb_wr(DDRPHY_REG(0x9007e),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x9007f),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90080),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x90081),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90082),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x90083),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90084),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x90085),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90086),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x90087),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90088),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90089),0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9008a),0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9008b),0x149);
	ddrphy_apb_wr(DDRPHY_REG(0x9008c),0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9008d),0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9008e),0x159);
	ddrphy_apb_wr(DDRPHY_REG(0x9008f),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90090),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90091),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90092),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90093),0x3c0);
	ddrphy_apb_wr(DDRPHY_REG(0x90094),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90095),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90096),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90097),0x48);
	ddrphy_apb_wr(DDRPHY_REG(0x90098),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x90099),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x9009a),0x58);
	ddrphy_apb_wr(DDRPHY_REG(0x9009b),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x9009c),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x9009d),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9009e),0x2);
	ddrphy_apb_wr(DDRPHY_REG(0x9009f),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900a0),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900a1),0x5);
	ddrphy_apb_wr(DDRPHY_REG(0x900a2),0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a3),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900a4),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900a5),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900a6),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x40000),0x811);
	ddrphy_apb_wr(DDRPHY_REG(0x40020),0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x40040),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40060),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40001),0x4016);
	ddrphy_apb_wr(DDRPHY_REG(0x40021),0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40041),0x4f);
	ddrphy_apb_wr(DDRPHY_REG(0x40061),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40002),0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x40022),0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40042),0x51);
	ddrphy_apb_wr(DDRPHY_REG(0x40062),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40003),0x811);
	ddrphy_apb_wr(DDRPHY_REG(0x40023),0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x40043),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40063),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40004),0x720);
	ddrphy_apb_wr(DDRPHY_REG(0x40024),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40044),0x1740);
	ddrphy_apb_wr(DDRPHY_REG(0x40064),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40005),0x16);
	ddrphy_apb_wr(DDRPHY_REG(0x40025),0x83);
	ddrphy_apb_wr(DDRPHY_REG(0x40045),0x4b);
	ddrphy_apb_wr(DDRPHY_REG(0x40065),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40006),0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40026),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40046),0x2001);
	ddrphy_apb_wr(DDRPHY_REG(0x40066),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40007),0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40027),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40047),0x2800);
	ddrphy_apb_wr(DDRPHY_REG(0x40067),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40008),0x716);
	ddrphy_apb_wr(DDRPHY_REG(0x40028),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40048),0xf00);
	ddrphy_apb_wr(DDRPHY_REG(0x40068),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40009),0x720);
	ddrphy_apb_wr(DDRPHY_REG(0x40029),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x40049),0x1400);
	ddrphy_apb_wr(DDRPHY_REG(0x40069),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000a),0xe08);
	ddrphy_apb_wr(DDRPHY_REG(0x4002a),0xc15);
	ddrphy_apb_wr(DDRPHY_REG(0x4004a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000b),0x623);
	ddrphy_apb_wr(DDRPHY_REG(0x4002b),0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x4004b),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006b),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000c),0x4004);
	ddrphy_apb_wr(DDRPHY_REG(0x4002c),0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x4004c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000d),0xe08);
	ddrphy_apb_wr(DDRPHY_REG(0x4002d),0xc1a);
	ddrphy_apb_wr(DDRPHY_REG(0x4004d),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006d),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000e),0x623);
	ddrphy_apb_wr(DDRPHY_REG(0x4002e),0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x4004e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4000f),0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x4002f),0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x4004f),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4006f),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40010),0x2604);
	ddrphy_apb_wr(DDRPHY_REG(0x40030),0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40050),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40070),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40011),0x708);
	ddrphy_apb_wr(DDRPHY_REG(0x40031),0x5);
	ddrphy_apb_wr(DDRPHY_REG(0x40051),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40071),0x2002);
	ddrphy_apb_wr(DDRPHY_REG(0x40012),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x40032),0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x40052),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40072),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40013),0x2604);
	ddrphy_apb_wr(DDRPHY_REG(0x40033),0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40053),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40073),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40014),0x708);
	ddrphy_apb_wr(DDRPHY_REG(0x40034),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x40054),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40074),0x2002);
	ddrphy_apb_wr(DDRPHY_REG(0x40015),0x4040);
	ddrphy_apb_wr(DDRPHY_REG(0x40035),0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x40055),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40075),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40016),0x60a);
	ddrphy_apb_wr(DDRPHY_REG(0x40036),0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40056),0x1200);
	ddrphy_apb_wr(DDRPHY_REG(0x40076),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40017),0x61a);
	ddrphy_apb_wr(DDRPHY_REG(0x40037),0x15);
	ddrphy_apb_wr(DDRPHY_REG(0x40057),0x1300);
	ddrphy_apb_wr(DDRPHY_REG(0x40077),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40018),0x60a);
	ddrphy_apb_wr(DDRPHY_REG(0x40038),0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40058),0x1200);
	ddrphy_apb_wr(DDRPHY_REG(0x40078),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x40019),0x642);
	ddrphy_apb_wr(DDRPHY_REG(0x40039),0x1a);
	ddrphy_apb_wr(DDRPHY_REG(0x40059),0x1300);
	ddrphy_apb_wr(DDRPHY_REG(0x40079),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4001a),0x4808);
	ddrphy_apb_wr(DDRPHY_REG(0x4003a),0x880);
	ddrphy_apb_wr(DDRPHY_REG(0x4005a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x4007a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a7),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900a8),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900a9),0x11a);
	ddrphy_apb_wr(DDRPHY_REG(0x900aa),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900ab),0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x900ac),0x2a);
	ddrphy_apb_wr(DDRPHY_REG(0x900ad),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900ae),0x7b2);
	ddrphy_apb_wr(DDRPHY_REG(0x900af),0x2a);
	ddrphy_apb_wr(DDRPHY_REG(0x900b0),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900b1),0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x900b2),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x900b3),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900b4),0x2a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900b5),0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x900b6),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900b7),0x370);
	ddrphy_apb_wr(DDRPHY_REG(0x900b8),0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x900b9),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x900ba),0x3c8);
	ddrphy_apb_wr(DDRPHY_REG(0x900bb),0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x900bc),0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x900bd),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900be),0x199);
	ddrphy_apb_wr(DDRPHY_REG(0x900bf),0x14);
	ddrphy_apb_wr(DDRPHY_REG(0x900c0),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900c1),0x11a);
	ddrphy_apb_wr(DDRPHY_REG(0x900c2),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900c3),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x900c4),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x900c5),0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x900c6),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900c7),0x199);
	ddrphy_apb_wr(DDRPHY_REG(0x900c8),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900c9),0x8568);
	ddrphy_apb_wr(DDRPHY_REG(0x900ca),0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x900cb),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x900cc),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x900cd),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900ce),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900cf),0x1d8);
	ddrphy_apb_wr(DDRPHY_REG(0x900d0),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900d1),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x900d2),0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x900d3),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900d4),0x70);
	ddrphy_apb_wr(DDRPHY_REG(0x900d5),0x788);
	ddrphy_apb_wr(DDRPHY_REG(0x900d6),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900d7),0x1ff8);
	ddrphy_apb_wr(DDRPHY_REG(0x900d8),0x85a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900d9),0x1e8);
	ddrphy_apb_wr(DDRPHY_REG(0x900da),0x50);
	ddrphy_apb_wr(DDRPHY_REG(0x900db),0x798);
	ddrphy_apb_wr(DDRPHY_REG(0x900dc),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900dd),0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x900de),0x7a0);
	ddrphy_apb_wr(DDRPHY_REG(0x900df),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900e0),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900e1),0x8310);
	ddrphy_apb_wr(DDRPHY_REG(0x900e2),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900e3),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x900e4),0xa310);
	ddrphy_apb_wr(DDRPHY_REG(0x900e5),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900e6),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x900e7),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900e8),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900e9),0x6e);
	ddrphy_apb_wr(DDRPHY_REG(0x900ea),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900eb),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x900ec),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900ed),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x900ee),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x900ef),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f0),0x8310);
	ddrphy_apb_wr(DDRPHY_REG(0x900f1),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900f2),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x900f3),0xa310);
	ddrphy_apb_wr(DDRPHY_REG(0x900f4),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x900f5),0x1ff8);
	ddrphy_apb_wr(DDRPHY_REG(0x900f6),0x85a8);
	ddrphy_apb_wr(DDRPHY_REG(0x900f7),0x1e8);
	ddrphy_apb_wr(DDRPHY_REG(0x900f8),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x900f9),0x798);
	ddrphy_apb_wr(DDRPHY_REG(0x900fa),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900fb),0x78);
	ddrphy_apb_wr(DDRPHY_REG(0x900fc),0x7a0);
	ddrphy_apb_wr(DDRPHY_REG(0x900fd),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x900fe),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x900ff),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x90100),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x90101),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90102),0x8b10);
	ddrphy_apb_wr(DDRPHY_REG(0x90103),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90104),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90105),0xab10);
	ddrphy_apb_wr(DDRPHY_REG(0x90106),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90107),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x90108),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90109),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x9010a),0x58);
	ddrphy_apb_wr(DDRPHY_REG(0x9010b),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9010c),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x9010d),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9010e),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x9010f),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90110),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90111),0x8b10);
	ddrphy_apb_wr(DDRPHY_REG(0x90112),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90113),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90114),0xab10);
	ddrphy_apb_wr(DDRPHY_REG(0x90115),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90116),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90117),0x1d8);
	ddrphy_apb_wr(DDRPHY_REG(0x90118),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90119),0x80);
	ddrphy_apb_wr(DDRPHY_REG(0x9011a),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x9011b),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x9011c),0x18);
	ddrphy_apb_wr(DDRPHY_REG(0x9011d),0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x9011e),0x6a);
	ddrphy_apb_wr(DDRPHY_REG(0x9011f),0xa);
	ddrphy_apb_wr(DDRPHY_REG(0x90120),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90121),0x1e9);
	ddrphy_apb_wr(DDRPHY_REG(0x90122),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90123),0x8080);
	ddrphy_apb_wr(DDRPHY_REG(0x90124),0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x90125),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x90126),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90127),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90128),0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x90129),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9012a),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x9012b),0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x9012c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9012d),0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x9012e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9012f),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90130),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90131),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90132),0x8080);
	ddrphy_apb_wr(DDRPHY_REG(0x90133),0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x90134),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90135),0x7aa);
	ddrphy_apb_wr(DDRPHY_REG(0x90136),0x6a);
	ddrphy_apb_wr(DDRPHY_REG(0x90137),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90138),0x8568);
	ddrphy_apb_wr(DDRPHY_REG(0x90139),0x108);
	ddrphy_apb_wr(DDRPHY_REG(0x9013a),0xb7);
	ddrphy_apb_wr(DDRPHY_REG(0x9013b),0x790);
	ddrphy_apb_wr(DDRPHY_REG(0x9013c),0x16a);
	ddrphy_apb_wr(DDRPHY_REG(0x9013d),0x1d);
	ddrphy_apb_wr(DDRPHY_REG(0x9013e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9013f),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90140),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90141),0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x90142),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x90143),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x90144),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x90145),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x90146),0xc);
	ddrphy_apb_wr(DDRPHY_REG(0x90147),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90148),0x68);
	ddrphy_apb_wr(DDRPHY_REG(0x90149),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9014a),0x408);
	ddrphy_apb_wr(DDRPHY_REG(0x9014b),0x169);
	ddrphy_apb_wr(DDRPHY_REG(0x9014c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9014d),0x8558);
	ddrphy_apb_wr(DDRPHY_REG(0x9014e),0x168);
	ddrphy_apb_wr(DDRPHY_REG(0x9014f),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90150),0x3c8);
	ddrphy_apb_wr(DDRPHY_REG(0x90151),0x1a9);
	ddrphy_apb_wr(DDRPHY_REG(0x90152),0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x90153),0x370);
	ddrphy_apb_wr(DDRPHY_REG(0x90154),0x129);
	ddrphy_apb_wr(DDRPHY_REG(0x90155),0x20);
	ddrphy_apb_wr(DDRPHY_REG(0x90156),0x2aa);
	ddrphy_apb_wr(DDRPHY_REG(0x90157),0x9);
	ddrphy_apb_wr(DDRPHY_REG(0x90158),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90159),0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x9015a),0x10e);
	ddrphy_apb_wr(DDRPHY_REG(0x9015b),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9015c),0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x9015d),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9015e),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9015f),0x8140);
	ddrphy_apb_wr(DDRPHY_REG(0x90160),0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x90161),0x10);
	ddrphy_apb_wr(DDRPHY_REG(0x90162),0x8138);
	ddrphy_apb_wr(DDRPHY_REG(0x90163),0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x90164),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90165),0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x90166),0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x90167),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90168),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90169),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9016a),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9016b),0x448);
	ddrphy_apb_wr(DDRPHY_REG(0x9016c),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9016d),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x9016e),0x7c0);
	ddrphy_apb_wr(DDRPHY_REG(0x9016f),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90170),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90171),0xe8);
	ddrphy_apb_wr(DDRPHY_REG(0x90172),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90173),0x47);
	ddrphy_apb_wr(DDRPHY_REG(0x90174),0x630);
	ddrphy_apb_wr(DDRPHY_REG(0x90175),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90176),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90177),0x618);
	ddrphy_apb_wr(DDRPHY_REG(0x90178),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x90179),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x9017a),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x9017b),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9017c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9017d),0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x9017e),0x109);
	ddrphy_apb_wr(DDRPHY_REG(0x9017f),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90180),0x8140);
	ddrphy_apb_wr(DDRPHY_REG(0x90181),0x10c);
	ddrphy_apb_wr(DDRPHY_REG(0x90182),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90183),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x90184),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90185),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90186),0x4);
	ddrphy_apb_wr(DDRPHY_REG(0x90187),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90188),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90189),0x7c8);
	ddrphy_apb_wr(DDRPHY_REG(0x9018a),0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x90006),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90007),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x90008),0x8);
	ddrphy_apb_wr(DDRPHY_REG(0x90009),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000a),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000b),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0xd00e7),0x400);
	ddrphy_apb_wr(DDRPHY_REG(0x90017),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9001f),0x2a);
	ddrphy_apb_wr(DDRPHY_REG(0x90026),0x6a);
	ddrphy_apb_wr(DDRPHY_REG(0x400d0),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x400d1),0x101);
	ddrphy_apb_wr(DDRPHY_REG(0x400d2),0x105);
	ddrphy_apb_wr(DDRPHY_REG(0x400d3),0x107);
	ddrphy_apb_wr(DDRPHY_REG(0x400d4),0x10f);
	ddrphy_apb_wr(DDRPHY_REG(0x400d5),0x202);
	ddrphy_apb_wr(DDRPHY_REG(0x400d6),0x20a);
	ddrphy_apb_wr(DDRPHY_REG(0x400d7),0x20b);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a),0x2);
	ddrphy_apb_wr(DDRPHY_REG(0x9000c),0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x9000d),0x173);
	ddrphy_apb_wr(DDRPHY_REG(0x9000e),0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x9000f),0x6110);
	ddrphy_apb_wr(DDRPHY_REG(0x90010),0x2152);
	ddrphy_apb_wr(DDRPHY_REG(0x90011),0xdfbd);
	ddrphy_apb_wr(DDRPHY_REG(0x90012),0x60);
	ddrphy_apb_wr(DDRPHY_REG(0x90013),0x6152);
	ddrphy_apb_wr(DDRPHY_REG(0x20010),0x5a);
	ddrphy_apb_wr(DDRPHY_REG(0x20011),0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x40080),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40081),0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40082),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40083),0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40084),0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40085),0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x400fd),0xf);
	ddrphy_apb_wr(DDRPHY_REG(0x10011),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10012),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10013),0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x10018),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x10002),0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x100b2),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x101b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x102b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x103b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x104b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x105b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x106b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x107b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x108b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11011),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11012),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11013),0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x11018),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x11002),0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x110b2),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x111b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x112b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x113b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x114b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x115b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x116b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x117b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x118b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12011),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12012),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12013),0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x12018),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x12002),0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x120b2),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x121b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x122b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x123b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x124b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x125b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x126b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x127b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x128b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13011),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13012),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13013),0x180);
	ddrphy_apb_wr(DDRPHY_REG(0x13018),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x13002),0x6209);
	ddrphy_apb_wr(DDRPHY_REG(0x130b2),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x131b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x132b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x133b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x134b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x135b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x136b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x137b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x138b4),0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a),0x2);
	ddrphy_apb_wr(DDRPHY_REG(0xc0080),0x2);
	ddrphy_apb_wr(DDRPHY_REG(0xd0000),0x1);             
#endif
	return;
}

//void ddr_init()
//{
//	volatile unsigned int readVal;
//
//	/* Todo: Setup the power to DDRC & DDR PHY */
//
//	/* Set the DDRC clock frequency, according to "JiaXing config PLL sequence 0.3.pdf" */
//	set_ddrc_freq(800);
//
//
//
//	//init sequence
//	reg32_write(0x190d208,0x10000000);
//	debug("ddr ctrl init\n");
//	reg32_write(0x190d10c,0x20000000);
//
//	debug("ddr ctrl init\n");
//	/* Configure DRAM controller registers */
//	dram_controller_cfg();
//	debug("ddr ctrl init\n");
//	reg32_write(0x0190d10c,0xFFFFFFFF);
//	// reg32_write(DDRC_BASE_REG(0x28), 0x1);                
//	debug("ddr ctrl init\n");
//	/* Todo: Release DDRC, DDR phy reset */
//	
//	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);		      
//	reg32_write(DDRC_BASE_REG(0x30), 0x000000a8);           
//
//	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x33f) != 0x223) 
//		;
//	debug("DDR: under self-refresh node\n");
//
//#if 0
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//	// reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000050);
//	/* LPDDR4 PHY config and training */
//	dram_phy_cfg();
//
////	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000070);           
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
//
//	while ((reg32_read(DDRC_BASE_REG(0x1BC)) & 0x1) == 0x0)  
//		;
//	debug("DDR: DFISTAT.dfi_init_complete\n");
//
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//	// reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010); 	//tyrion           
//	// reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);   //tyrion         
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000050);    	//tyrion        
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000051);		//tyrion 
//	// reg32_write(DDRC_BASE_REG(0x30), 0x00000088);    //tyrion   
//	reg32_write(DDRC_BASE_REG(0x30), 0x00000000);      //tyrion   
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
//	while ((reg32_read(DDRC_BASE_REG(0x324)) & 0x1) == 0x0)  
//		;
//	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x3) != 0x1)    
//		;
//    debug("DDR: return to normal state\n");
//
//    reg32_write(DDRC_BASE_REG(0x30), 0x00000000);      //tyrion   
//    reg32_write(DDRC_BASE_REG(0x30), 0x00000000);      //tyrion   
//	// reg32_write(DDRC_BASE_REG(0x30), 0x000000002);        //tyrion       
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
//	reg32_write(DDRC_BASE_REG(0x490), 0x00000001); 
//
//	reg32_write(DDRC_BASE_REG(0x540), 0x00000001);  	//tyrion 
//
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
////	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
//	readVal += 0;
//
//	debug("DDR: init full done!\n");
//
//#else
//
//#define BOOT_FROM_FREQ1
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//#if defined(BOOT_FROM_FREQ0)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);    
//#elif defined(BOOT_FROM_FREQ1)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000110);       
//#elif defined(BOOT_FROM_FREQ2)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000210);       
//#elif defined(BOOT_FROM_FREQ3)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000310);       
//#endif
//	/* LPDDR4 PHY config and training */
//	// dram_phy_cfg();
// //    //jianle_phy_train();
// //    jiaxing_ddr_phy();
//    dram_phy_cfg();
//    
//	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//#if defined(BOOT_FROM_FREQ0)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000030);           
//#elif defined(BOOT_FROM_FREQ1)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000130);     
//#elif defined(BOOT_FROM_FREQ2)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000230);
//#elif defined(BOOT_FROM_FREQ3)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000330);
//#endif
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
//
//	while ((reg32_read(DDRC_BASE_REG(0x1BC)) & 0x1) == 0x0);
//
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
//#if defined(BOOT_FROM_FREQ0)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);           
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);           
//#elif defined(BOOT_FROM_FREQ1)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000110);         
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000111);         
//#elif defined(BOOT_FROM_FREQ2)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000210);         
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000211);   
//#elif defined(BOOT_FROM_FREQ3)
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000310);         
//	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000311);   
//#endif
//
//	reg32_write(DDRC_BASE_REG(0x30), 0x00000088);            
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
//	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
//	while ((reg32_read(DDRC_BASE_REG(0x324)) & 0x1) == 0x0)  
//		;
//	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x3) != 0x1)    
//		;
//
//	reg32_write(DDRC_BASE_REG(0x30), 0x00000088);            
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
//	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);           
//	readVal = reg32_read(DDRC_BASE_REG(0xCC));
//	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
//	readVal += 0;
//#endif
//	reg32_write(0x80000000, 0x5d5dd5d5);
//	debug("read ddr 0x%8x", reg32_read(0x80000000));
//}
