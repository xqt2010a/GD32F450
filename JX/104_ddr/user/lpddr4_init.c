//#include "spl.h"
#include "ddr.h"
#include "dv_ddr_code.h"

//#define BOOT_FROM_FREQ0    //3200M
#define BOOT_FROM_FREQ1    //800M
//#define BOOT_FROM_FREQ2    //1600M
//#define BOOT_FROM_FREQ3    //2400M

static void ddrphy_apb_wr(unsigned long addr, unsigned long val)
{
	reg32_write(addr, val);
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
	//printf("[PMU Major message = 0x%08x]\n", mail);
	return;
}

static inline void decode_streaming_message(void)
{
	unsigned int string_index, arg;
	int i = 0;

	string_index = get_stream_message();
	//printf("	PMU String index = 0x%08x\n", string_index);
	while (i < (string_index & 0xffff)){
		arg = get_stream_message();
		arg += 0;
		//printf("	arg[%d] = 0x%08x\n", i, arg);
		i++;
	}

	//printf("\n");
}

int wait_ddrphy_training_complete(void)
{
	unsigned int mail;
	int ret = 0;
	while (1) {
		mail = get_mail();
		decode_major_message(mail);
		if (mail == 0x08) {
			decode_streaming_message();
		} else if (mail == 0x07) {
			//printf("Training PASS\n");
			break;
		} else if (mail == 0xff) {
			//printf("Training FAILED\n");
			ret = 1;
			break;
		}
	}
	return ret;
}

void set_ddrc_freq(void)
{
	reg32_write(0x0190d02c, 0x0);
	reg32_write(0x0190d068, 0x80008000);
	reg32_write(0x0190d000, 0xf0);
#if defined(BOOT_FROM_FREQ0)
	/* Set the PHY input clocks back to pstate 0 -- 800MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d000, 0x0C1909f0);
	reg32_write(0x0190d000, 0x0C190900);
#elif defined(BOOT_FROM_FREQ1)
	/* Set the PHY input clocks back to pstate 0 -- 200MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d000, 0x0C0649f0);
	reg32_write(0x0190d000, 0x0C064900);
#elif defined(BOOT_FROM_FREQ2)
	/* Set the PHY input clocks back to pstate 0 -- 400MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d000, 0x0C0C89f0);
	reg32_write(0x0190d000, 0x0C0C8900);
#elif defined(BOOT_FROM_FREQ3)
	/* Set the PHY input clocks back to pstate 0 -- 600MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d000, 0x0C12C9f0);
	reg32_write(0x0190d000, 0x0C12C900);
#endif
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
		;
	reg32_write(0x0190d02c, 0x1);

	return;
}

void dram_controller_cfg(void)
{
	reg32_write(DDRC_BASE_REG(0x304), 0x00000001);           
	reg32_write(DDRC_BASE_REG(0x30), 0x00000001);            
	reg32_write(DDRC_BASE_REG(0x0), 0xA3080020);             
	/* Freq0 settings (3200M data rate) */
	reg32_write(DDRC_BASE_REG(0x28), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x20), 0x00000203);            
	reg32_write(DDRC_BASE_REG(0x24), 0x0186A000);            
	reg32_write(DDRC_BASE_REG(0x64), 0x006100E0);            
	reg32_write(DDRC_BASE_REG(0xD0), 0xC003061B);            
	reg32_write(DDRC_BASE_REG(0xD4), 0x009D0000);            
	reg32_write(DDRC_BASE_REG(0xDC), 0x00D4002D);            
	reg32_write(DDRC_BASE_REG(0xE0), 0x00330008);            
	reg32_write(DDRC_BASE_REG(0xE8), 0x0066004A);            
	reg32_write(DDRC_BASE_REG(0xEC), 0x0016004A);            
	
	reg32_write(DDRC_BASE_REG(0x100), 0x1A201B22);           
	reg32_write(DDRC_BASE_REG(0x104), 0x00060633);           
	reg32_write(DDRC_BASE_REG(0x108), 0x070E1214);           
	reg32_write(DDRC_BASE_REG(0x10C), 0x00C0C000);           
	reg32_write(DDRC_BASE_REG(0x110), 0x0F04080F);           
	reg32_write(DDRC_BASE_REG(0x114), 0x02040C0C);           
	reg32_write(DDRC_BASE_REG(0x118), 0x01010007);           
	reg32_write(DDRC_BASE_REG(0x11C), 0x00000401);           
	reg32_write(DDRC_BASE_REG(0x130), 0x00020600);           
	reg32_write(DDRC_BASE_REG(0x134), 0x0C100002);           
	reg32_write(DDRC_BASE_REG(0x138), 0x000000E6);           
	reg32_write(DDRC_BASE_REG(0x144), 0x00A00050);           

	reg32_write(DDRC_BASE_REG(0x180), 0x03200018);           
	reg32_write(DDRC_BASE_REG(0x184), 0x028061A8);           
	reg32_write(DDRC_BASE_REG(0x188), 0x00000000);
	
	reg32_write(DDRC_BASE_REG(0x190), 0x0497820A);           
	reg32_write(DDRC_BASE_REG(0x194), 0x00080303);           
	reg32_write(DDRC_BASE_REG(0x1A0), 0xE0400018);           
	reg32_write(DDRC_BASE_REG(0x1A4), 0x00DF00E4);           
	reg32_write(DDRC_BASE_REG(0x1A8), 0x80000000);           
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);           
	reg32_write(DDRC_BASE_REG(0x1B4), 0x0000170A);           
	reg32_write(DDRC_BASE_REG(0x1C0), 0x00000001);           
	reg32_write(DDRC_BASE_REG(0x1C4), 0x00000001);	         
	
	reg32_write(DDRC_BASE_REG(0xF4), 0x00000639);            
	reg32_write(DDRC_BASE_REG(0x200), 0x00000015);           
	reg32_write(DDRC_BASE_REG(0x208), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x20C), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x210), 0x00001F1F);           
	reg32_write(DDRC_BASE_REG(0x204), 0x00080808);           
	reg32_write(DDRC_BASE_REG(0x214), 0x07070707);           
	reg32_write(DDRC_BASE_REG(0x218), 0x0f080707);           
	reg32_write(DDRC_BASE_REG(0x21C), 0x0000000F);           
	reg32_write(DDRC_BASE_REG(0x224), 0x07070707);           
	reg32_write(DDRC_BASE_REG(0x228), 0x07070707);           

	/* Freq1 settings (800M data rate) */
	reg32_write(DDRC_FREQ1_REG(0x20), 0x00000001);           
	reg32_write(DDRC_FREQ1_REG(0x24), 0x0061A800);           
	reg32_write(DDRC_FREQ1_REG(0x50), 0x0020D040);           
	reg32_write(DDRC_FREQ1_REG(0x64), 0x00180038);           
	reg32_write(DDRC_FREQ1_REG(0xDC), 0x00940009);           
	reg32_write(DDRC_FREQ1_REG(0xE0), 0x00310000);           
	reg32_write(DDRC_FREQ1_REG(0xE8), 0x0066004A);           
	reg32_write(DDRC_FREQ1_REG(0xEC), 0x0016004A);           
	reg32_write(DDRC_FREQ1_REG(0x100), 0x0C080609);          
	reg32_write(DDRC_FREQ1_REG(0x104), 0x0003040D);          
	reg32_write(DDRC_FREQ1_REG(0x108), 0x0305090C);          
	reg32_write(DDRC_FREQ1_REG(0x10C), 0x00505000);          
	reg32_write(DDRC_FREQ1_REG(0x110), 0x04040204);          
	reg32_write(DDRC_FREQ1_REG(0x114), 0x02030303);          
	reg32_write(DDRC_FREQ1_REG(0x118), 0x01010004);          
	reg32_write(DDRC_FREQ1_REG(0x11C), 0x00000301);          
	reg32_write(DDRC_FREQ1_REG(0x130), 0x00020300);          
	reg32_write(DDRC_FREQ1_REG(0x134), 0x0A100002);          
	reg32_write(DDRC_FREQ1_REG(0x138), 0x0000003A);          
	reg32_write(DDRC_FREQ1_REG(0x144), 0x00280014);          
	reg32_write(DDRC_FREQ1_REG(0x180), 0x00C80006);          
	reg32_write(DDRC_FREQ1_REG(0x190), 0x03858202);          
	reg32_write(DDRC_FREQ1_REG(0x194), 0x00080303);          
	reg32_write(DDRC_FREQ1_REG(0x1B4), 0x00000502);          

	/* Freq2 settings (1600M data rate) */
	reg32_write(DDRC_FREQ2_REG(0x20), 0x00000001);           
	reg32_write(DDRC_FREQ2_REG(0x24), 0x00C35000);           
	reg32_write(DDRC_FREQ2_REG(0x50), 0x0020D040);           
	reg32_write(DDRC_FREQ2_REG(0x64), 0x00300070);           
	reg32_write(DDRC_FREQ2_REG(0xDC), 0x00940009);           
	reg32_write(DDRC_FREQ2_REG(0xE0), 0x00310000);           
	reg32_write(DDRC_FREQ2_REG(0xE8), 0x0066004A);           
	reg32_write(DDRC_FREQ2_REG(0xEC), 0x0016004A);           
	reg32_write(DDRC_FREQ2_REG(0x100), 0x0F100D11);          
	reg32_write(DDRC_FREQ2_REG(0x104), 0x0003041A);          
	reg32_write(DDRC_FREQ2_REG(0x108), 0x04070B0D);          
	reg32_write(DDRC_FREQ2_REG(0x10C), 0x00606000);          
	reg32_write(DDRC_FREQ2_REG(0x110), 0x08040408);          
	reg32_write(DDRC_FREQ2_REG(0x114), 0x02030606);          
	reg32_write(DDRC_FREQ2_REG(0x118), 0x01010004);          
	reg32_write(DDRC_FREQ2_REG(0x11C), 0x00000301);          
	reg32_write(DDRC_FREQ2_REG(0x130), 0x00020300);          
	reg32_write(DDRC_FREQ2_REG(0x134), 0x0A100002);          
	reg32_write(DDRC_FREQ2_REG(0x138), 0x00000073);          
	reg32_write(DDRC_FREQ2_REG(0x144), 0x00500028);          
	reg32_write(DDRC_FREQ2_REG(0x180), 0x0190000C);          
	reg32_write(DDRC_FREQ2_REG(0x190), 0x03898204);          
	reg32_write(DDRC_FREQ2_REG(0x194), 0x00080303);          
	reg32_write(DDRC_FREQ2_REG(0x1B4), 0x00000904);          

	/* Freq3 settings (2400M data rate) */
	reg32_write(DDRC_FREQ3_REG(0x20), 0x00000203);           
	reg32_write(DDRC_FREQ3_REG(0x24), 0x0124F800);           
	reg32_write(DDRC_FREQ3_REG(0x50), 0x0020D040);           
	reg32_write(DDRC_FREQ3_REG(0x64), 0x004900A9);           
	reg32_write(DDRC_FREQ3_REG(0xDC), 0x00940009);           
	reg32_write(DDRC_FREQ3_REG(0xE0), 0x00310000);           
	reg32_write(DDRC_FREQ3_REG(0xE8), 0x0066004A);           
	reg32_write(DDRC_FREQ3_REG(0xEC), 0x0016004A);           
	reg32_write(DDRC_FREQ3_REG(0x100), 0x1319141A);          
	reg32_write(DDRC_FREQ3_REG(0x104), 0x00050526);          
	reg32_write(DDRC_FREQ3_REG(0x108), 0x060C1111);          
	reg32_write(DDRC_FREQ3_REG(0x10C), 0x00909000);          
	reg32_write(DDRC_FREQ3_REG(0x110), 0x0B04070B);          
	reg32_write(DDRC_FREQ3_REG(0x114), 0x02040A0A);          
	reg32_write(DDRC_FREQ3_REG(0x118), 0x01010006);          
	reg32_write(DDRC_FREQ3_REG(0x11C), 0x00000401);          
	reg32_write(DDRC_FREQ3_REG(0x130), 0x00020500);          
	reg32_write(DDRC_FREQ3_REG(0x134), 0x0B100002);          
	reg32_write(DDRC_FREQ3_REG(0x138), 0x000000AD);          
	reg32_write(DDRC_FREQ3_REG(0x144), 0x0079003D);          
	reg32_write(DDRC_FREQ3_REG(0x180), 0x02590013);          
	reg32_write(DDRC_FREQ3_REG(0x190), 0x03998208);          
	reg32_write(DDRC_FREQ3_REG(0x194), 0x00080303);          
	reg32_write(DDRC_FREQ3_REG(0x1B4), 0x00001308);   

	reg32_write(DDRC_BASE_REG(0x244), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x250), 0x29001505);           
	reg32_write(DDRC_BASE_REG(0x254), 0x0000002C);           
	reg32_write(DDRC_BASE_REG(0x25C), 0x5900575B);           
	reg32_write(DDRC_BASE_REG(0x264), 0x00000009);           
	reg32_write(DDRC_BASE_REG(0x26C), 0x02005574);           

	reg32_write(DDRC_BASE_REG(0x300), 0x00000016);           
	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x30C), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
	
	reg32_write(DDRC_BASE_REG(0x36C), 0x00000011);           
	reg32_write(DDRC_BASE_REG(0x400), 0x00000111);           
	reg32_write(DDRC_BASE_REG(0x404), 0x000010F3);           
	reg32_write(DDRC_BASE_REG(0x408), 0x000072FF);		       
	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);           
	reg32_write(DDRC_BASE_REG(0x494), 0x01110D00);           
	reg32_write(DDRC_BASE_REG(0x498), 0x00620790);           
	reg32_write(DDRC_BASE_REG(0x49C), 0x00100001);           
	reg32_write(DDRC_BASE_REG(0x4A0), 0x0000041F);           

	return;
}

void dram_phy_cfg(void)
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

	ddrphy_apb_wr(DDRPHY_REG(0x11005f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11015f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11105f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11115f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11205f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11215f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11305f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x11315f), 0x1ff);              

	ddrphy_apb_wr(DDRPHY_REG(0x21005f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21015f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21105f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21115f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21205f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21215f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21305f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x21315f), 0x1ff);              

	ddrphy_apb_wr(DDRPHY_REG(0x31005f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31015f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31105f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31115f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31205f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31215f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31305f), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x31315f), 0x1ff);   

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
	ddrphy_apb_wr(DDRPHY_REG(0x200c5), 0x19);                
	ddrphy_apb_wr(DDRPHY_REG(0x1200c5), 0x7);                
	ddrphy_apb_wr(DDRPHY_REG(0x2200c5), 0xb); 
	ddrphy_apb_wr(DDRPHY_REG(0x3200c5), 0xa); 
	ddrphy_apb_wr(DDRPHY_REG(0x2002e), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x12002e), 0x1);         
	ddrphy_apb_wr(DDRPHY_REG(0x22002e), 0x1); 
	ddrphy_apb_wr(DDRPHY_REG(0x32002e), 0x2); 
	ddrphy_apb_wr(DDRPHY_REG(0x90204), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x190204), 0x0);                
	ddrphy_apb_wr(DDRPHY_REG(0x290204), 0x0);  
	ddrphy_apb_wr(DDRPHY_REG(0x390204), 0x0);  

	ddrphy_apb_wr(DDRPHY_REG(0x20024), 0xe3);                
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x120024), 0xa3);               
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);            
	ddrphy_apb_wr(DDRPHY_REG(0x220024), 0xa3);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);
	ddrphy_apb_wr(DDRPHY_REG(0x320024), 0xa3);
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);

	ddrphy_apb_wr(DDRPHY_REG(0x20056), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x120056), 0xa);                
	ddrphy_apb_wr(DDRPHY_REG(0x220056), 0xa);                 
	ddrphy_apb_wr(DDRPHY_REG(0x320056), 0x2);

	ddrphy_apb_wr(DDRPHY_REG(0x1004d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1014d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1104d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1114d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1204d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1214d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1304d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x1314d), 0xe00);               
	ddrphy_apb_wr(DDRPHY_REG(0x11004d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11014d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11104d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11114d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11204d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11214d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11304d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x11314d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21004d), 0xe00); 
	ddrphy_apb_wr(DDRPHY_REG(0x21014d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21104d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21114d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21204d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21214d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21304d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x21314d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31004d), 0xe00); 
	ddrphy_apb_wr(DDRPHY_REG(0x31014d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31104d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31114d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31204d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31214d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31304d), 0xe00);              
	ddrphy_apb_wr(DDRPHY_REG(0x31314d), 0xe00);     

	ddrphy_apb_wr(DDRPHY_REG(0x10049), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x10149), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x11049), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x11149), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x12049), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x12149), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x13049), 0xfbe);               
	ddrphy_apb_wr(DDRPHY_REG(0x13149), 0xfbe);               

	ddrphy_apb_wr(DDRPHY_REG(0x110049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x110149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x111049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x111149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x112049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x112149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x113049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x113149), 0xfbe);              

	ddrphy_apb_wr(DDRPHY_REG(0x210049), 0xfbe); 
	ddrphy_apb_wr(DDRPHY_REG(0x210149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x211049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x211149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x212049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x212149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x213049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x213149), 0xfbe);   
	
	ddrphy_apb_wr(DDRPHY_REG(0x310049), 0xfbe);
	ddrphy_apb_wr(DDRPHY_REG(0x310149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x311049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x311149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x312049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x312149), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x313049), 0xfbe);              
	ddrphy_apb_wr(DDRPHY_REG(0x313149), 0xfbe);   
	
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
	ddrphy_apb_wr(DDRPHY_REG(0x20008), 0x320);               
	ddrphy_apb_wr(DDRPHY_REG(0x120008), 0xc8);              
	ddrphy_apb_wr(DDRPHY_REG(0x220008), 0x190);
	ddrphy_apb_wr(DDRPHY_REG(0x320008), 0x258);
	ddrphy_apb_wr(DDRPHY_REG(0x20088), 0x9);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200b2), 0x104);               
	ddrphy_apb_wr(DDRPHY_REG(0x10043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x10143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x1200b2), 0x104);              
	ddrphy_apb_wr(DDRPHY_REG(0x110043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x110143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x111043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x111143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x112043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x112143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x113043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x113143), 0x5a1);             
	ddrphy_apb_wr(DDRPHY_REG(0x2200b2), 0x104);    
	ddrphy_apb_wr(DDRPHY_REG(0x210043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x210143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x211043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x211143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x212043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x212143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x213043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x213143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x3200b2), 0x104);      
	ddrphy_apb_wr(DDRPHY_REG(0x310043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x310143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x311043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x311143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x312043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x312143), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x313043), 0x5a1);              
	ddrphy_apb_wr(DDRPHY_REG(0x313143), 0x5a1);      
	ddrphy_apb_wr(DDRPHY_REG(0x200fa), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x1200fa), 0x1);           
	ddrphy_apb_wr(DDRPHY_REG(0x2200fa), 0x1);  
	ddrphy_apb_wr(DDRPHY_REG(0x3200fa), 0x1);  
	ddrphy_apb_wr(DDRPHY_REG(0x20019), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x120019), 0x1);       
	ddrphy_apb_wr(DDRPHY_REG(0x220019), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x320019), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0x200f0), 0x600);
	ddrphy_apb_wr(DDRPHY_REG(0x200f1), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f2), 0x4444);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f3), 0x8888);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f4), 0x5655);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f5), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f6), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f7), 0xf000);              
	ddrphy_apb_wr(DDRPHY_REG(0x2000b), 0x64);                
	ddrphy_apb_wr(DDRPHY_REG(0x2000c), 0xc8);                
	ddrphy_apb_wr(DDRPHY_REG(0x2000d), 0x7d0);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000e), 0x2c);                
	ddrphy_apb_wr(DDRPHY_REG(0x12000b), 0x19);               
	ddrphy_apb_wr(DDRPHY_REG(0x12000c), 0x32);               
	ddrphy_apb_wr(DDRPHY_REG(0x12000d), 0x1F4);              
	ddrphy_apb_wr(DDRPHY_REG(0x12000e), 0x10);               
	ddrphy_apb_wr(DDRPHY_REG(0x22000b), 0x33); 
	ddrphy_apb_wr(DDRPHY_REG(0x22000c), 0x65);               
	ddrphy_apb_wr(DDRPHY_REG(0x22000d), 0x3e9);              
	ddrphy_apb_wr(DDRPHY_REG(0x22000e), 0x2c);        
	ddrphy_apb_wr(DDRPHY_REG(0x32000b), 0x4c);   
	ddrphy_apb_wr(DDRPHY_REG(0x32000c), 0x97);               
	ddrphy_apb_wr(DDRPHY_REG(0x32000d), 0x5dd);              
	ddrphy_apb_wr(DDRPHY_REG(0x32000e), 0x2c);  
	ddrphy_apb_wr(DDRPHY_REG(0x20025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x2002d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x12002d), 0x0);                
	ddrphy_apb_wr(DDRPHY_REG(0x22002d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x32002d), 0x0);         

	/* Load the 1D IMEM image */
	//ddrphy_apb_wr(DDRPHY_REG(0x20060), 0x2);               
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	//ddr_load_train_code(FW_1D_IMAGE);     //labc                   
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	/* Set the PHY input clocks for pstate 0 -- 800MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d02c, 0x0);
	reg32_write(0x0190d068, 0x80008000);
	reg32_write(0x0190d000, 0xf0);
	reg32_write(0x0190d000, 0x0C1909f0);
	reg32_write(0x0190d000, 0x0C190900);
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
		;
	reg32_write(0x0190d02c, 0x1);

	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0xc80);               
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x1E1E);              
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x11);                
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x131f);              
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0xC8);                
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x310);               
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x2dd4);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x33);                
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x2dd4);              
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x33);                
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x1000);              
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0xd400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x332d);              
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0xd400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x332d);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
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
	//ddr_load_train_code(FW_2D_IMAGE);     //labc                       
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	/* 3200 mts 2D training */
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0xc80);               
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x1E1E);              
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x11);                
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x61);                
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0xC8);                
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x100);               
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x1f7f);              
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x310);               
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x2dd4);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x33);                
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x2dd4);              
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x33);                
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x1000);              
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0xd400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x332d);              
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0xd400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x332d);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
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

	/* Step (E) Set the PHY input clocks for pstate 1 */
	/* Set the PHY frequency to 200MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d02c, 0x0);
	reg32_write(0x0190d068, 0x80008000);
	reg32_write(0x0190d000, 0xf0);
	reg32_write(0x0190d000, 0x0C0649f0);
	reg32_write(0x0190d000, 0x0C064900);
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
		;
	reg32_write(0x0190d02c, 0x1);

	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	//ddr_load_train_code(FW_1D_IMAGE);     //labc                       
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	//printf("pstate=1: set dfi clk done done\n");
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x320);               
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x1E1E);              
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x11);                
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x121f);              
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0xc8);                
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x310);               
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x914);               
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31);                
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x6);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x914);               
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);                
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x1000);              
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x1400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3109);              
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x1400);              
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3109);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
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

	/* Step (E2) Set the PHY input clocks for pstate 2 */
	/* Set the PHY frequency to 400MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d02c, 0x0);             
	reg32_write(0x0190d068, 0x80008000);
	reg32_write(0x0190d000, 0xf0);
	reg32_write(0x0190d000, 0x0C0C89f0);
	reg32_write(0x0190d000, 0x0C0C8900);
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
		;
	reg32_write(0x0190d02c, 0x1);

	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	//ddr_load_train_code(FW_1D_IMAGE);     //labc                     
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	//printf("pstate=1: set dfi clk done done\n");
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x640);               
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x1E1E);              
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x11);                
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x121f);              
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0xc8);                
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x310);               
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x1224); 
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31); 
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x6);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x1224);
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);  
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x1000);              
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x2400);             
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3112); 
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x2400);     
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3112);          
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
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

	/* Step (E3) Set the PHY input clocks for pstate 3 */
	/* Set the PHY frequency to 600MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	reg32_write(0x0190d02c, 0x0);             
	reg32_write(0x0190d068, 0x80008000);
	reg32_write(0x0190d000, 0xf0);
	reg32_write(0x0190d000, 0x0C12C9f0);
	reg32_write(0x0190d000, 0x0C12C900);
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
		;
	reg32_write(0x0190d02c, 0x1);

	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	//ddr_load_train_code(FW_1D_IMAGE);     //labc                        
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 

	//printf("pstate=1: set dfi clk done done\n");
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x960);               
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x1E1E);              
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x11);                
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x121f);              
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0xc8);                
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x310);               
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x2444);  
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31);     
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x6);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x2444);    
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);        
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a66);              
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a08);              
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x16);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x1000);              
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x3);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x4400);            
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3124);   
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x4400);            
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3124);            
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x6600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x084a);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);                
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1600);              
	ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
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

    set_ddrc_freq();

	/* (I) Load PHY Init Engine Image */
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
	ddrphy_apb_wr(DDRPHY_REG(0x2000b), 0x64);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000c), 0xc8);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000d), 0x7d0);              
	ddrphy_apb_wr(DDRPHY_REG(0x2000e), 0x2c);               
	ddrphy_apb_wr(DDRPHY_REG(0x12000b), 0x19);              
	ddrphy_apb_wr(DDRPHY_REG(0x12000c), 0x32);              
	ddrphy_apb_wr(DDRPHY_REG(0x12000d), 0x1F4);             
	ddrphy_apb_wr(DDRPHY_REG(0x12000e), 0x10);              
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
	ddrphy_apb_wr(DDRPHY_REG(0x120010), 0x5a);                 
	ddrphy_apb_wr(DDRPHY_REG(0x120011), 0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x220010), 0x5a);                 
	ddrphy_apb_wr(DDRPHY_REG(0x220011), 0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x320010), 0x5a);                 
	ddrphy_apb_wr(DDRPHY_REG(0x320011), 0x3);
	ddrphy_apb_wr(DDRPHY_REG(0x40080), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40081), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40082), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40083), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x40084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x40085), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x140080), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x140081), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x140082), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x140083), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x140084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x140085), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x240080), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x240081), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x240082), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x240083), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x240084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x240085), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x340080), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x340081), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x340082), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x340083), 0x12);
	ddrphy_apb_wr(DDRPHY_REG(0x340084), 0xe0);
	ddrphy_apb_wr(DDRPHY_REG(0x340085), 0x12);
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
	ddrphy_apb_wr(DDRPHY_REG(0x20089), 0x1);                  
	ddrphy_apb_wr(DDRPHY_REG(0x20088), 0x19);                 
	ddrphy_apb_wr(DDRPHY_REG(0xc0080), 0x2);                  
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                  

	return;
}

#define JX_DDR(x)  (*(unsigned int *)(x))


void jiaxing_smu_ddr_init(void)
{
	unsigned int i, val, val1;
    JX_DDR(0x8FFF0) = 0x1524;
    val = JX_DDR(0x0190d02c);
	JX_DDR(0x0190d02c) = val & 0x7FFFFFFF;
    val = JX_DDR(0x0190d068);
	JX_DDR(0x0190d068) = val | 0x80008000;
    val = JX_DDR(0x0190d000);
	JX_DDR(0x0190d000) = (val & 0xFFFFFF00) | 0xF0;
    val = JX_DDR(0x0190d000);
	//JX_DDR(0x0190d000) = (val  & 0x000000FF) | 0x0c064900;
    val1 = (3<<26)|(100<<14)|(1<<11)|(1<<8);
    JX_DDR(0x0190d000) = (val  & 0x000000FF) | val1;
    
    val = JX_DDR(0x0190d000);
	JX_DDR(0x0190d000) = val  & 0xFFFFFF00;
	JX_DDR(0x8FFF4) = 0x2;
	//while(0 == (JX_DDR(0x0190d080) & 0x80000000));
	for(i=0; i<0xFFFFF; i++);
    val = JX_DDR(0x0190d02c);
	JX_DDR(0x0190d02c) = val | 0x80000000;
	JX_DDR(0x8FFF0) = 0x1;
}

void ddr_init(void)
{
	volatile unsigned int readVal;

	/* Todo: Setup the power to DDRC & DDR PHY */

	/* Set the DDRC clock frequency, according to "JiaXing config PLL sequence 0.3.pdf" */
	//set_ddrc_freq();
	//jiaxing_smu_ddr_init();
    	/*AddNew*/
	reg32_write(0x0190d208,0x1<<28);				//DDRPowerOn
	reg32_write(0x0190d10c,0x1<<29);				//DDR_APB_RST
	/* Configure DRAM controller registers */
	dram_controller_cfg();
    reg32_write(0x0190d10c,0xFFFFFFFF);
	//jiaxing_dram_controller_cfg();
	//ddr_sequence();
	//reg32_write(DDRC_BASE_REG(0x28), 0x1);                

	/* Todo: Release DDRC, DDR phy reset */
	
	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);		      
	reg32_write(DDRC_BASE_REG(0x30), 0x000000a8);           

	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x33f) != 0x223);

	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
#if defined(BOOT_FROM_FREQ0)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);    
#elif defined(BOOT_FROM_FREQ1)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000110);       
#elif defined(BOOT_FROM_FREQ2)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000210);       
#elif defined(BOOT_FROM_FREQ3)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000310);       
#endif
	/* LPDDR4 PHY config and training */
	//dram_phy_cfg();
    jiaxing_ddr_phy();
    
	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
#if defined(BOOT_FROM_FREQ0)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000030);           
#elif defined(BOOT_FROM_FREQ1)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000130);     
#elif defined(BOOT_FROM_FREQ2)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000230);
#elif defined(BOOT_FROM_FREQ3)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000330);
#endif
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           

	while ((reg32_read(DDRC_BASE_REG(0x1BC)) & 0x1) == 0x0);

	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
#if defined(BOOT_FROM_FREQ0)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);           
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);           
#elif defined(BOOT_FROM_FREQ1)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000110);         
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000111);         
#elif defined(BOOT_FROM_FREQ2)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000210);         
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000211);   
#elif defined(BOOT_FROM_FREQ3)
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000310);         
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000311);   
#endif

	reg32_write(DDRC_BASE_REG(0x30), 0x00000088);            
	readVal = reg32_read(DDRC_BASE_REG(0xCC));
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           
	while ((reg32_read(DDRC_BASE_REG(0x324)) & 0x1) == 0x0)  
		;
	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x3) != 0x1)    
		;

	reg32_write(DDRC_BASE_REG(0x30), 0x00000088);            
	readVal = reg32_read(DDRC_BASE_REG(0xCC));
	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);           
	readVal = reg32_read(DDRC_BASE_REG(0xCC));
	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
	readVal += 0;
	
	return;
}
