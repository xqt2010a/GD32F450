#include "ddr.h"
#include "uart.h"

#define SKIP_TRAIN   0
#define TRAIN_2D     0

   


void ddrphy_apb_wr(unsigned long addr, unsigned long val)
{
	reg32_write(addr, val);
}


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
	printf("[PMU Major message = 0x%08x]\n", mail);
	return;
}

static inline void decode_streaming_message(void)
{
	unsigned int string_index, arg;
	int i = 0;

	string_index = get_stream_message();
	printf("	PMU String index = 0x%08x\n", string_index);
	while (i < (string_index & 0xffff)){
		arg = get_stream_message();
		arg += 0;
		printf("	arg[%d] = 0x%08x\n", i, arg);
		i++;
	}

	printf("\n");
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
			printf("Training PASS\n");
			break;
		} else if (mail == 0xff) {
			printf("Training FAILED\n");
			ret = 1;
			break;
		}
	}
	return ret;
}

void set_ddrc_freq(u32 freq_MHZ)
{
	reg32_write(0x0190d02c, 0x0);           //Ryan: make all clock mux select 24M
	reg32_write(0x0190d068, 0x80008000);    //Ryan: Release DDR Pin strap
	reg32_write(0x0190d000, 0xf0);          //Ryan: DDR PLL PowerDown and Bypass Mode(24M)
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
	while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000);  //Ryan: polling DDR PLL lock signal
		// printf("..\n");
	reg32_write(0x0190d02c, 0x1<<31);       //Ryan: make DDR clock from DDR PLL

	return;
}

void dram_controller_cfg(void)
{
	reg32_write(DDRC_BASE_REG(0x304), 0x00000001);  //Ryan: When 1, No Reads or Writes are issued to SDRAM as long as this is asserted.       
	reg32_write(DDRC_BASE_REG(0x30), 0x00000001);   //Ryan: CTL puts the SDRAM into Self Refresh     
  
	//reg32_write(DDRC_BASE_REG(0x0), 0xA1080020); //pengfei
	reg32_write(DDRC_BASE_REG(0x0), 0x81080020); //pengfei
	//reg32_write(DDRC_BASE_REG(0x0), 0xA3080020);    
													/*Ryan: [31:30]: 10   (x16 device)
															[29]   :  1   (Choose target_frequency register)
															[28:24]:  0   (only one rank, dont care this)
															[23]   :  0   (Reserved)
															[22]   :  0   (freq_ratio 1:2 Mode)
															[21:20]: 11   (dont care)
															[19:16]: 1000 (Burst length of 16, For LPDDR4, dont change)
															[15]   :  0   (dont care)
															[14]   :  0   (Reserved)
															[13:12]: 00   (data_bus_width: Full DQ bus width to SDRAM)
															[11: 9]: 000  (dont care)
															[8]    :  0   (Sequential busrt mode, For LPDDR4, must 0)
															[7:6]  : 00   (Reserved)
															[5:0]  : 100000  (LPDDR4 device in use)
													 */	                                                        	                                                             
	reg32_write(DDRC_BASE_REG(0x28), 0x00000000);   //Ryan: [1:0]  : 00 (Target frequency is FREQ0)    

	/* Only Freq0 settings (800M data rate) */
	reg32_write(DDRC_BASE_REG(0x20), 0x00000001);   //Ryan: DERATEEN         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x24), 0x0061A800);   //Ryan: DERATEINT  		//NEED CHECK
	reg32_write(DDRC_BASE_REG(0x50), 0x0020D040);   //Ryan: RFSHCTL0		//NEED CHECK
	//reg32_write(DDRC_BASE_REG(0x64), 0x00180024);   //Ryan: RFSHTMG         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x64), 0x00180038);           
	//reg32_write(DDRC_BASE_REG(0xD0), 0xC0020188);   //Ryan: INT0         	
	reg32_write(DDRC_BASE_REG(0xD0), 0xC003061B);            
	reg32_write(DDRC_BASE_REG(0xD4), 0x009D0000);   //Ryan: INT1         
	reg32_write(DDRC_BASE_REG(0xDC), 0x00940009);   //Ryan: INT3         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0xE0), 0x00310000);   //Ryan: INT4         	//NEED CHECK
	//reg32_write(DDRC_BASE_REG(0xE8), 0x0055004A);   //Ryan: INT6         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0xE8), 0x0066004A);           
	//reg32_write(DDRC_BASE_REG(0xEC), 0x0015004A);   //Ryan: INT7         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0xEC), 0x0016004A);           
	
	reg32_write(DDRC_BASE_REG(0x100), 0x0C080609);  //Ryan: DRAMTMG0         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x104), 0x0003040D);  //Ryan: DRAMTMG1         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x108), 0x0305090C);  //Ryan: DRAMTMG2         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x10C), 0x00505000);  //Ryan: DRAMTMG3         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x110), 0x04040204);  //Ryan: DRAMTMG4         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x114), 0x02030303);  //Ryan: DRAMTMG5         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x118), 0x01010004);  //Ryan: DRAMTMG6         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x11C), 0x00000301);  //Ryan: DRAMTMG7         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x130), 0x00020300);  //Ryan: DRAMTMG12         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x134), 0x0A100002);  //Ryan: DRAMTMG13         //NEED CHECK
	//reg32_write(DDRC_BASE_REG(0x138), 0x00000026);  //Ryan: DRAMTMG14         //NEED CHECK
	reg32_write(DDRC_BASE_REG(0x138), 0x0000003A);          
	reg32_write(DDRC_BASE_REG(0x144), 0x00280014);  //Ryan: DRAMTMG17         //NEED CHECK

	reg32_write(DDRC_BASE_REG(0x180), 0x00C80006);  //Ryan: ZQCTL0         		//NEED CHECK
	//reg32_write(DDRC_BASE_REG(0x184), 0x00A0186A);  //Ryan: ZQCTL1         
	reg32_write(DDRC_BASE_REG(0x184), 0x028061A8);           
	reg32_write(DDRC_BASE_REG(0x188), 0x00000000);  //Ryan: ZQCTL2
	
	reg32_write(DDRC_BASE_REG(0x190), 0x03858202);  //Ryan: DFITMG0         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0x194), 0x00080303);  //Ryan: DFITMG1         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0x1A0), 0xE0400018);  //Ryan: DFIUPD0         
	reg32_write(DDRC_BASE_REG(0x1A4), 0x00DF00E4);  //Ryan: DFIUPD1         
	reg32_write(DDRC_BASE_REG(0x1A8), 0x80000000);  //Ryan: DFIUPD2         
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000011);  //Ryan: DFIMISC         
	//reg32_write(DDRC_BASE_REG(0x1B4), 0x00000502);  //Ryan: DFITMG2         	//NEED CHECK
	reg32_write(DDRC_BASE_REG(0x1B4), 0x0000170A);           
	reg32_write(DDRC_BASE_REG(0x1C0), 0x00000001);  //Ryan: DBICTL         
	reg32_write(DDRC_BASE_REG(0x1C4), 0x00000001);	//Ryan: DFIPHYMSTR         
	
	reg32_write(DDRC_BASE_REG(0xF4), 0x00000639);   //Ryan: RANKCTL         
	//reg32_write(DDRC_BASE_REG(0x200), 0x0000001f);  //Ryan: ADDRMAP0         
	reg32_write(DDRC_BASE_REG(0x200), 0x00000015);           
	reg32_write(DDRC_BASE_REG(0x204), 0x00080808);  //Ryan: ADDRMAP1        
	reg32_write(DDRC_BASE_REG(0x208), 0x00000000);  //Ryan: ADDRMAP2         
	reg32_write(DDRC_BASE_REG(0x20C), 0x00000000);  //Ryan: ADDRMAP3         
	reg32_write(DDRC_BASE_REG(0x210), 0x00001F1F);  //Ryan: ADDRMAP4         
	reg32_write(DDRC_BASE_REG(0x214), 0x07070707);  //Ryan: ADDRMAP5         
	//reg32_write(DDRC_BASE_REG(0x218), 0x0f070707);  //Ryan: ADDRMAP6         
	reg32_write(DDRC_BASE_REG(0x218), 0x0f080707);           
	reg32_write(DDRC_BASE_REG(0x21C), 0x0000000F);  //Ryan: ADDRMAP7         
	reg32_write(DDRC_BASE_REG(0x224), 0x07070707);  //Ryan: ADDRMAP9         
	reg32_write(DDRC_BASE_REG(0x228), 0x07070707);  //Ryan: ADDRMAP10          

	reg32_write(DDRC_BASE_REG(0x244), 0x00000000);  //Ryan: ODTMAP         
	reg32_write(DDRC_BASE_REG(0x250), 0x29001505);  //Ryan: SCHED         
	reg32_write(DDRC_BASE_REG(0x254), 0x0000002C);  //Ryan: SCHED1         
	reg32_write(DDRC_BASE_REG(0x25C), 0x5900575B);  //Ryan: PERFHPR1         
	reg32_write(DDRC_BASE_REG(0x264), 0x00000009);  //Ryan: PERFLPR1         
	reg32_write(DDRC_BASE_REG(0x26C), 0x02005574);  //Ryan: PERFWR1         

	//reg32_write(DDRC_BASE_REG(0x300), 0x00000000);  //Ryan: DBG0         
	reg32_write(DDRC_BASE_REG(0x300), 0x00000016);           
	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);  //Ryan: DBG1         
	reg32_write(DDRC_BASE_REG(0x30C), 0x00000000);  //Ryan: DBGCMD         
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);  //Ryan: DBGSTAT         
	
	reg32_write(DDRC_BASE_REG(0x36C), 0x00000011);  //Ryan: POISONCFG         
	reg32_write(DDRC_BASE_REG(0x400), 0x00000111);  //Ryan: PCCFG         
	reg32_write(DDRC_BASE_REG(0x404), 0x000010F3);  //Ryan: PCFGR_n         
	reg32_write(DDRC_BASE_REG(0x408), 0x000072FF);	//Ryan: PCFGW_n	       
	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);  //Ryan: PCTRL_n         
	reg32_write(DDRC_BASE_REG(0x494), 0x01110D00);  //Ryan: PCFGQOS0_n         
	reg32_write(DDRC_BASE_REG(0x498), 0x00620790);  //Ryan: PCFGQOS1_n         
	reg32_write(DDRC_BASE_REG(0x49C), 0x00100001);  //Ryan: PCFGWQOS0_n        
	reg32_write(DDRC_BASE_REG(0x4A0), 0x0000041F);  //Ryan: PCFGWQOS1_n         

	return;
}

void phyinit_C_initPhyConfig(void)
{
	//Ryan: 
	//##############################################################
	//
	// Step (C) Initialize PHY Configuration 
	//
	// Load the required PHY configuration registers for the appropriate mode and memory configuration
	//
	//##############################################################
	
	/*Ryan: Programming TxSlewRate::TxPreDrvMode to 0x1
			Programming TxSlewRate::TxPreP       to 0xf
			Programming TxSlewRate::TxPreN       to 0xf                  
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x1005f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1015f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1105f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1115f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1205f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1215f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1305f), 0x1ff);               
	ddrphy_apb_wr(DDRPHY_REG(0x1315f), 0x1ff);               

	/*Ryan: Programming TxSlewRate::TxPreDrvMode to 0x1, ANIB=0~9
			Programming TxSlewRate::TxPreP       to 0xf, ANIB=0~9
			Programming TxSlewRate::TxPreN       to 0xf, ANIB=0~9                  
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x55), 0x1ff);                  
	ddrphy_apb_wr(DDRPHY_REG(0x1055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x2055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x3055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x4055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x5055), 0x1ff);                
	ddrphy_apb_wr(DDRPHY_REG(0x6055), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x7055), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x8055), 0x1ff);              
	ddrphy_apb_wr(DDRPHY_REG(0x9055), 0x1ff);
	
	/*Ryan: Programming PllCtrl2 to 0x19(3200M/4), 0x7(800M/4), 0xb(1600M/4), 0xa(2400M/4)
					Dficlk    Dficlk    | PllFreqSel
					min(MHz)  max (MHz) | [4:0]     
					================================
					937.5     1067      | 11000��b  //0x18
					625       937.5     | 11001��b  //0x19
					468.75    625       | 01010��b  //0xa
					312.5     468.75    | 01011��b  //0xb
					234.4     312.5     | 00110��b  //0x6
					166       234.4     | 00111��b  //0x7
	*/              
	ddrphy_apb_wr(DDRPHY_REG(0x200c5), 0x7);                
	ddrphy_apb_wr(DDRPHY_REG(0x200c7), 0x100); //Ryan: Need additional configuration
	
	/*Ryan: Programming ARdPtrInitVal to 0x1
					PLL Bypassed mode:
						For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 2-6
						For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
					
					PLL Enabled mode:
						For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
						For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 0-6
	*/	
	ddrphy_apb_wr(DDRPHY_REG(0x2002e), 0x1);  //DDR800/2       

	/*Ryan: Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOff  to 0x0
			Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOn   to 0x0
	*/	
	ddrphy_apb_wr(DDRPHY_REG(0x90204), 0x0);                 

	/*Ryan: Programming DqsPreambleControl::TwoTckRxDqsPre to 0x1
					Programming DqsPreambleControl::TwoTckTxDqsPre to 0x1
					Programming DqsPreambleControl::PositionDfeInit to 0x2
					Programming DqsPreambleControl::LP4TglTwoTckTxDqsPre to 0x1
					Programming DqsPreambleControl::LP4PostambleExt to 0x1
					Programming DqsPreambleControl::LP4SttcPreBridgeRxEn to 0x1
					
					Programming DqsPreambleControl to 0xeb
					
					DqsPreambleControl[5]: LP4TglTwoTckTxDqsPre:
																	0: level first-memclk preamble
														  1: toggling first-memclk preamble

					Programming DbyteDllModeCntrl to 0x0
					DbyteDllModeCntrl[1]: DllRxPreambleMode
																	Must be set to 1 if read DQS preamble contains a toggle,
																	for example DDR4 or LPDDR4 read toggling preambe mode					
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x20024), 0xa3);   //Ryan: LP4TglTwoTckTxDqsPre bit is 1
	ddrphy_apb_wr(DDRPHY_REG(0x2003a), 0x0);    //Ryan: Need to confirm             

	/*Ryan: Programming ProcOdtTimeCtl to 0xa
					Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x20056), 0xa);                 

	/*Ryan: Programming TxOdtDrvStren::ODTStrenP to 0x0
					Programming TxOdtDrvStren::ODTStrenN to 0x18 (60ohms)
					
					TxOdtDrvStren[ 5:0]: ODTStrenP
					TxOdtDrvStren[11:6]: ODTStrenN
																000_00_0 = High Impedance
																000_00_1 = 480 ohms
																000_01_0 = 240 ohms
																000_01_1 = 160 ohms
																001_00_0 = 120 ohms
																001_00_1 = 96.0 ohms
																001_01_0 = 80.0 ohms
																001_01_1 = 68.6 ohms
																011_00_0 = 60.0 ohms
																011_00_1 = 53.3 ohms
																011_01_0 = 48.0 ohms
																011_01_1 = 43.6 ohms
																111_00_0 = 40.0 ohms
																111_00_1 = 36.9 ohms
																111_01_0 = 34.3 ohms
																111_01_1 = 32.0 ohms
																111_11_0 = 30.0 ohms
																111_11_1 = 28.2 ohms
				  Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x1004d), 0x600);  //Ryan: N(60ohms)  P(High Impedance)             
	ddrphy_apb_wr(DDRPHY_REG(0x1014d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1104d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1114d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1204d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1214d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1304d), 0x600);               
	ddrphy_apb_wr(DDRPHY_REG(0x1314d), 0x600);               

	/*Ryan: Programming TxImpedanceCtrl1::DrvStrenFSDqP to 0x18 (60ohms)
					Programming TxImpedanceCtrl1::DrvStrenFSDqN to 0x18 (60ohms)
				  Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x10049), 0x69a);  //Ryan: N(48ohms) P(48ohms)           
	ddrphy_apb_wr(DDRPHY_REG(0x10149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x11049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x11149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x12049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x12149), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x13049), 0x69a);               
	ddrphy_apb_wr(DDRPHY_REG(0x13149), 0x69a);               
	
	/*Ryan: Programming ATxImpedance::ADrvStrenP to 0x1 (60 ohms)
					Programming ATxImpedance::ADrvStrenN to 0x1 (60 ohms)
					
					ATxImpedance[4:0]: ADrvStrenP
					ATxImpedance[9:5]: ADrvStrenN
															00000 = 120.0 Ohm
															00001 = 60.0 Ohm
															00011 = 40.0 Ohm
															00111 = 30.0 Ohm
															01111 = 24.0 Ohm
															11111 = 20.0 Ohm
				  Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x43), 0x63);    //Ryan: N(60 ohms) P(60 ohms)                
	ddrphy_apb_wr(DDRPHY_REG(0x1043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x2043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x3043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x4043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5043), 0x63);                 
	ddrphy_apb_wr(DDRPHY_REG(0x6043), 0x63);  //Ryan: Need to confirm write this or not               
	ddrphy_apb_wr(DDRPHY_REG(0x7043), 0x63);  //Ryan: Need to confirm write this or not                      
	ddrphy_apb_wr(DDRPHY_REG(0x8043), 0x63);  //Ryan: Need to confirm write this or not                      
	ddrphy_apb_wr(DDRPHY_REG(0x9043), 0x63);  //Ryan: Need to confirm write this or not                      

	/*Ryan: Programming DfiMode to 0x1
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x20018), 0x1);  //Ryan: we only use dfi0
	
	/*Ryan: Programming DfiCAMode to 0x4
  */             
	ddrphy_apb_wr(DDRPHY_REG(0x20075), 0x4);  
	
	/*Ryan: Programming CalDrvStr0::CalDrvStrPd50 to 0x0
					Programming CalDrvStr0::CalDrvStrPu50 to 0x0
	*/	               
	ddrphy_apb_wr(DDRPHY_REG(0x20050), 0x0);    
				 
	/*Ryan: Programming CalUclkInfo::CalUClkTicksPer1uS to 0xc8
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x20008), 0xc8);              
	
	/*Ryan: Programming CalRate::CalInterval to 0x9
					Programming CalRate::CalOnce to 0x0	
	*/                 
	ddrphy_apb_wr(DDRPHY_REG(0x20088), 0x9);

	/*Ryan: Programming VrefInGlobal::GlobalVrefInSel to 0x4
					Programming VrefInGlobal::GlobalVrefInDAC to 0x33 (0.25*VDDQ)
					Programming VrefInGlobal to 0x19c
					
					VrefInGlobal[9:3]: GlobalVrefInDAC
															RANGE0 : DDR3,DDR4,LPDDR3 [GlobalVrefInSel[2] = 0]
															DAC Output Voltage = GlobalVrefInDAC == 6'h00 ? Hi-Z : 0.345*VDDQ +(0.005*GlobalVrefInDAC)*VDDQ				
															==========================================================
															RANGE1 : LPDDR4 [GlobalVrefInSel[2] = 1]
															DAC Output Voltage = GlobalVrefInDAC == 6'h00 ? Hi-Z : (0.005*(GlobalVrefInDAC-1))*VDDQ
					Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x200b2), 0x104);  //Ryan: RANGE1(0.155*VDDQ), RANGE0(0.5*VDDQ)
				   
	/*Ryan: Programming DqDqsRcvCntrl::MajorModeDbyte to 0x2
					Programming DqDqsRcvCntrl to 0x5a1
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x10043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x10143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x11143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x12143), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13043), 0x5a1);               
	ddrphy_apb_wr(DDRPHY_REG(0x13143), 0x5a1);
				  
	/*Ryan: Programming DfiFreqRatio_p0 to 0x1
	*/     
	ddrphy_apb_wr(DDRPHY_REG(0x200fa), 0x1);                 
	
	/*Ryan: Programming TristateModeCA::DisDynAdrTri_p0 to 0x1
					Programming TristateModeCA::DDR2TMode_p0 to 0x0
	*/	
	ddrphy_apb_wr(DDRPHY_REG(0x20019), 0x1);                 
	
	/*Ryan: Programming DfiFreqXlat*
		dwc_ddrphy_apb_wr(32'h200f0,16'h0);
		dwc_ddrphy_apb_wr(32'h200f1,16'h0);
		dwc_ddrphy_apb_wr(32'h200f2,16'h4444);
		dwc_ddrphy_apb_wr(32'h200f3,16'h8888);
		dwc_ddrphy_apb_wr(32'h200f4,16'h5555);
		dwc_ddrphy_apb_wr(32'h200f5,16'h0);
		dwc_ddrphy_apb_wr(32'h200f6,16'h0);
		dwc_ddrphy_apb_wr(32'h200f7,16'hf000);
		
		Need to confirm
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x200f0), 0x5550);  //Ryan: Need to confirm
	ddrphy_apb_wr(DDRPHY_REG(0x200f1), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f2), 0x4444);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f3), 0x8888);              
	ddrphy_apb_wr(DDRPHY_REG(0x200f4), 0x5655);  //Ryan: Need to confirm            
	ddrphy_apb_wr(DDRPHY_REG(0x200f5), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f6), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x200f7), 0xf000); 

	/*Ryan: Programming Seq0BDLY0 to 0x1a
					Programming Seq0BDLY1 to 0x33
					Programming Seq0BDLY2 to 0x1f5
					Programming Seq0BDLY3 to 0x10
					
					PHY Initialization Engine (PIE) Delay Register 0
					This register is available for selection by the NOP and WAIT instructions
					in the PIE for the delay value.
					Programmed by dwc_ddrphy_phyinit_I_loadPIEImage() to support frequency changes.
					Programming Seq0BDLY[3,2,1,0] Registers:
					-------------------------------
					These registers are used to control various delays during PLL, DLL initialization.
					These regiseters should be programmed with the corresponding number of MEMCLK
					cycles representing a fixed amount of delay irrespective of DfiClk Frequency.
					When calculating the number of cycles to be programmed for each PState, it
					should be noted that the sequencer multiplies programmed values by a factor of 8.
					Register  Required Delay Programmed Value
					--------- --------------- ------------------------------
					Seq0BDLY0 0.5us ( 0.5us / MEMCLK period) / 8
					Seq0BDLY1 1.0us ( 1.0us / MEMCLK period) / 8
					Seq0BDLY2 10.0us (10.0us / MEMCLK period) / 8
					64/4 if (MEMCLK Freq <= 400MHz)
					Seq0BDLY3 - 132/4 if (400MHz Freq < MEMCLK Freq <= 533MHz)
					176/4 if (MEMCLK Freq > 533MHz)

		Need to confirm
	*/	             
	ddrphy_apb_wr(DDRPHY_REG(0x2000b), 0x19);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000c), 0x32);               
	ddrphy_apb_wr(DDRPHY_REG(0x2000d), 0x1F4);              
	ddrphy_apb_wr(DDRPHY_REG(0x2000e), 0x10);               
	
	/*Ryan: Programming MasterX4Config::X4TG to 0x0
	  Need to confirm
	*/	  
	ddrphy_apb_wr(DDRPHY_REG(0x20025), 0x0);
	
	/*Ryan Programming DMIPinPresent::RdDbiEnabled to 0x0
	*/                 
	ddrphy_apb_wr(DDRPHY_REG(0x2002d), 0x0);   
	
	printf("DDR PHY: phyinit_C_initPhyConfig Complete\n");
			  
}

void phyinit_D_Load_1D_IMEM(void)
{  
	//Ryan
	//##############################################################
	//
	// (D) Load the 1D IMEM image
	// 
	// This function loads the training firmware IMEM image into the SRAM.
	// See PhyInit App Note for detailed description and function usage
	// 
	//##############################################################

	/*Ryan Programming MemResetL to 0x2
	*/
	ddrphy_apb_wr(DDRPHY_REG(0x20060), 0x2);
	
	/*Ryan 
				 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
					  This allows the memory controller unrestricted access to the configuration CSRs. 
	*/
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);

	/*Ryan Reading input file: dwc_lpddr4_multiphy_v2_tsmc28hpcp18/1.40a/firmware/A-2017.05/lpddr4/lpddr4_pmu_train_imem.bin
				 offset 0x50000 size 0x4000
	*/
	//ddr_load_train_code(FW_1D_IMAGE); //Ryan: Need to check
	ddr_load_train_code_jtag(TRAIN_1D);
	
	/*Ryan 
				 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
			  This allows the firmware unrestricted access to the configuration CSRs.
	*/ 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
}	

void phyinit_E_setDFIClk(void)
{
	//Ryan	
	//##############################################################
	//
	// Step (E) Set the PHY input clocks to the desired frequency for pstate 0 
	// 
	// See PhyInit App Note for detailed description and function usage
	// 
	//##############################################################
	
		   
	/* Step (E) Set the PHY input clocks for pstate 1 */
	/* Set the PHY frequency to 200MHz, according to "JiaXing config PLL sequence 0.3.pdf" */
	// reg32_write(0x0190d02c, 0x0);
	// reg32_write(0x0190d068, 0x80008000);
	// reg32_write(0x0190d000, 0xf0);
	// reg32_write(0x0190d000, 0x0C0649f0);
	// reg32_write(0x0190d000, 0x0C064900);
	// while ((reg32_read(0x0190d080) & 0x80000000) != 0x80000000)
	// 	;
	// reg32_write(0x0190d02c, 0x1);
}

void phyinit_F_Load_1D_DMEM(void)
{
	//Ryan
	//##############################################################
	//
	// Setp (F) Load the 1D DMEM image and write the 1D Message Block parameters for the training firmware 
	// 
	// The procedure is as follows: 
	// 
	//##############################################################

	/*Ryan 
				 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
					  This allows the memory controller unrestricted access to the configuration CSRs. 
	*/
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);

	/*Ryan Reading input file: dwc_lpddr4_multiphy_v2_tsmc28hpcp18/1.40a/firmware/A-2017.05/lpddr4/lpddr4_pmu_train_dmem.bin
				 offset 0x54000 size 0x32c
	*/
	//ddr_load_train_code(FW_1D_IMAGE); //Ryan: Need to check
	
	/*Ryan 
				 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
			  This allows the firmware unrestricted access to the configuration CSRs.
	*/ 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
}

void phyinit_F_Write_Message_Block(void)
{
  /*Ryan Update message block
  */
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);
					 
	ddrphy_apb_wr(DDRPHY_REG(0x54000), 0x0);    /*Ryan:    
																											uint8_t  Reserved00;    
																uint8_t  MsgMisc;
												*/
	ddrphy_apb_wr(DDRPHY_REG(0x54001), 0x0);    //Ryan: uint16_t PmuRevision;             
	ddrphy_apb_wr(DDRPHY_REG(0x54002), 0x0);    /*Ryan: uint8_t  Pstate;         //Need to confirm
																												0x0 = Pstate 0
																				0x1 = Pstate 1
																				0x2 = Pstate 2
																				0x3 = Pstate 3
																		  uint8_t  PllBypassEn;
																				0x0 = PHY PLL is enabled for target Pstate
																				0x1 = PHY PLL is bypassed for target Pstate
																							*/            
	ddrphy_apb_wr(DDRPHY_REG(0x54003), 0x320);  //Ryan: uint16_t DRAMFreq;            
	ddrphy_apb_wr(DDRPHY_REG(0x54004), 0x2);    /*Ryan: uint8_t  DfiFreqRatio;
																					Frequency ratio betwen DfiCtlClk and SDRAM memclk.
																					   0x1 = 1:1
																					   0x2 = 1:2
																					   0x4 = 1:4
																									uint8_t  BPZNResVal ;
																					Must be programmed to match the precision resistor connected to Phy BP_ZN
																					   0x0 = 240 Ohm (recommended value)
																					   0x1 = 120 Ohm
																					   0x2 = 40 Ohm
														  */       	    																							      
	ddrphy_apb_wr(DDRPHY_REG(0x54005), 0x3C3C); /*Ryan  uint8_t  PhyOdtImpedance;
																												 Must be programmed to the termination impedance in ohms used by PHY during reads. 	 																										             
																												 0x0 = Firmware skips programming (must be manually programmed by user prior to training start)	 																										             	 																										                            
																											uint8_t  PhyDrvImpedance;  
																												 Must be programmed to the driver impedance in ohms used by PHY during writes for all DBYTE drivers (DQ/DM/DBI/DQS).	 																										             
																												 0x0 = Firmware skips programming (must be manually programmed by user prior to training start)
																							*/				             
			  
	ddrphy_apb_wr(DDRPHY_REG(0x54006), 0x15);   /*Ryan: uint8_t  PhyVref;
																										 Receiver Vref = VDDQ*PhyVref[6:0]/128
																										 For example to set Vref at 0.25*VDDQ, set this field to 0x20.
																							  uint8_t  Lp4Misc;
																									 Lp4Misc[0] Enable dfi_reset_n
																			  */
	ddrphy_apb_wr(DDRPHY_REG(0x54007), 0x0);    /*Ryan: uint8_t  Reserved0E;
																											uint8_t  CsTestFail;
																					 This field will be set if training fails on any rank.
																						0x0 = No failures
																						non-zero = one or more ranks failed training																											
																							*/             
	ddrphy_apb_wr(DDRPHY_REG(0x54008), 0x131f); /*Ryan: uint16_t SequenceCtrl;
																					 If the bit is set to 1, the training step will run. 
																					 If the bit is set to 0, the training step will be skipped.
																					 
																					 Training step to bit mapping:
																						SequenceCtrl[0] = Run DevInit - Device/phy initialization. Should always be set.
																						SequenceCtrl[1] = Run WrLvl - Write leveling
																						SequenceCtrl[2] = Run RxEn - Read gate training
																						SequenceCtrl[3] = Run RdDQS1D - 1d read dqs training
																						SequenceCtrl[4] = Run WrDQ1D - 1d write dq training
																						SequenceCtrl[5] = RFU, must be zero
																						SequenceCtrl[6] = RFU, must be zero
																						SequenceCtrl[7] = RFU, must be zero
																						SequenceCtrl[8] = Run RdDeskew - Per lane read dq deskew training
																						SequenceCtrl[9] = Run MxRdLat - Max read latency training
																						SequenceCtrl[11-10] = RFU, must be zero
																						SequenceCtrl[12]      = Run LPCA - CA Training
																						SequenceCtrl[15-13] = RFU, must be zero
																						*/             
	ddrphy_apb_wr(DDRPHY_REG(0x54009), 0x04);   /*Ryan: uint8_t  HdtCtrl;
															0x04 = More Details
																				0x05 = Detailed debug messages (e.g. Eye delays)
																				0x0A = Coarse debug messages (e.g. rank information)
																				0xC8 = Stage completion
																				0xC9 = Assertion messages
																				0xFF = Firmware completion messages only
																						*/             
	ddrphy_apb_wr(DDRPHY_REG(0x5400a), 0x0);    //Ryan: Reserved             
	ddrphy_apb_wr(DDRPHY_REG(0x5400b), 0x2);    /*Ryan: uint8_t  DFIMRLMargin;
																									  Margin added to smallest passing trained DFI Max Read Latency value, in units of DFI clocks. 
																									  Recommended to be >= 1. See the Training App Note for more details on the training process and the use of this value.
																							*/              
	ddrphy_apb_wr(DDRPHY_REG(0x5400c), 0x1);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5400f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54010), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54011), 0x0);    /*Ryan: uint16_t PhyConfigOverride; 
																					0x0: Use hardware csr value for PhyConfing (recommended)
																					Other values: Use value for PhyConfig instead of Hardware value.
															*/  
																														   
	ddrphy_apb_wr(DDRPHY_REG(0x54012), 0x120);  /*Ryan: uint8_t  EnabledDQsChA;
																													Total number of DQ bits enabled in PHY Channel A
																										  uint8_t  CsPresentChA;
																												0x1 = CS0 is populated with DRAM
																												0x3 = CS0 and CS1 are populated with DRAM
																							*/             
	ddrphy_apb_wr(DDRPHY_REG(0x54013), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54014), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54015), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54016), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54017), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54018), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54019), 0x994);   //Ryan: Default is 0x914           
	ddrphy_apb_wr(DDRPHY_REG(0x5401a), 0x31);    //Ryan: Default is 0x2            
	ddrphy_apb_wr(DDRPHY_REG(0x5401b), 0x4a55);  //Ryan: Default is 0x4d64            
	ddrphy_apb_wr(DDRPHY_REG(0x5401c), 0x4a00);  //Ryan: Default is 0x4f28            
	ddrphy_apb_wr(DDRPHY_REG(0x5401d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5401e), 0x15);   //Ryan: Default is 0x0             
	ddrphy_apb_wr(DDRPHY_REG(0x5401f), 0x994);   //Ryan: Default is 0x914            
	ddrphy_apb_wr(DDRPHY_REG(0x54020), 0x31);    //Ryan: Default is 0x2            
	ddrphy_apb_wr(DDRPHY_REG(0x54021), 0x4a55);  //Ryan: Default is 0x4d64            
	ddrphy_apb_wr(DDRPHY_REG(0x54022), 0x4a00);  //Ryan: Default is 0x4f28            
	ddrphy_apb_wr(DDRPHY_REG(0x54023), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54024), 0x15);                
	ddrphy_apb_wr(DDRPHY_REG(0x54025), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54026), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54027), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54028), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54029), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402a), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402b), 0x0);    /*Ryan: uint8_t  EnabledDQsChB;
																													Total number of DQ bits enabled in PHY Channel B
																											uint8_t  CsPresentChB;
																													0x0 = No chip selects are populated with DRAM
																													0x1 = CS0 is populated with DRAM
																													0x3 = CS0 and CS1 are populated with DRAM
																													All other encodings are illegal
																							*/           
	ddrphy_apb_wr(DDRPHY_REG(0x5402c), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402d), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5402f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54030), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54031), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54032), 0x9400);  //Ryan: Default is 0x1400            
	ddrphy_apb_wr(DDRPHY_REG(0x54033), 0x3109);  //Ryan: Default is 0x209            
	ddrphy_apb_wr(DDRPHY_REG(0x54034), 0x5500);  //Ryan: Default is 0x6400            
	ddrphy_apb_wr(DDRPHY_REG(0x54035), 0x004a);  //Ryan: Default is 0x284d            
	ddrphy_apb_wr(DDRPHY_REG(0x54036), 0x4a);    //Ryan: Default is 0x4f            
	ddrphy_apb_wr(DDRPHY_REG(0x54037), 0x1500);  //Ryan: Default is 0x0             
	ddrphy_apb_wr(DDRPHY_REG(0x54038), 0x9400);  //Ryan: Default is 0x1400            
	ddrphy_apb_wr(DDRPHY_REG(0x54039), 0x3109);  //Ryan: Default is 0x209            
	ddrphy_apb_wr(DDRPHY_REG(0x5403a), 0x5500);  //Ryan: Default is 0x6400            
	ddrphy_apb_wr(DDRPHY_REG(0x5403b), 0x004a);  //Ryan: Default is 0x284d            
	ddrphy_apb_wr(DDRPHY_REG(0x5403c), 0x4a);    //Ryan: Default is 0x4f            
	ddrphy_apb_wr(DDRPHY_REG(0x5403d), 0x1500);  //Ryan: Default is 0x0            
	ddrphy_apb_wr(DDRPHY_REG(0x5403e), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x5403f), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54040), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54041), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54042), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54043), 0x0);                 
	ddrphy_apb_wr(DDRPHY_REG(0x54044), 0x0);    //Ryan: Need to add 0x44 address 
	
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);                 
}

void phyinit_G_execFW(void)
{
	//Ryan
	//##############################################################
	//
	// Step (G) Execute the Training Firmware 
	// 
	// The training firmware is executed with the following procedure: 
	//
	//##############################################################

	/*Ryan
		 1.  Reset the firmware microcontroller by writing the MicroReset CSR to set the StallToMicro and 
			 ResetToMicro fields to 1 (all other fields should be zero). 
			 Then rewrite the CSR so that only the StallToMicro remains set (all other fields should be zero).
	*/ 	                 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
	ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x9);                 
	ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1); 
	
	/*Ryan
	   2. Begin execution of the training firmware by setting the MicroReset CSR to 4'b0000. 
	*/                
	ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x0);                 

	/*Ryan
		 3. Wait for the training firmware to complete by following the procedure in "uCtrl Initialization and Mailbox Messaging"
	*/
	/* wait for train complete */
	wait_ddrphy_training_complete();  //Need to check
  
  /*Ryan
	 4. Halt the microcontroller.
  */
	ddrphy_apb_wr(DDRPHY_REG(0xd0099), 0x1);   
}	

void phyinit_H_Read_Message_Block(void)
{	
	//Ryan
	//##############################################################
	//
	// Step (H) Read the Message Block results
	// 
	// The procedure is as follows:
	// 
	//##############################################################
	
	/*Ryan
		 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	*/              
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);  
	
	/*Ryan
		 2. Read the Firmware Message Block to obtain the results from the training.
				This can be accomplished by issuing APB read commands to the DMEM addresses.
				Example:
				if (Train2D)
				{ 
				  _read_2d_message_block_outputs_
				}
				else
				{
				  _read_1d_message_block_outputs_
				}
				This can be accomplished by issuing APB read commands to the DMEM addresses.
	*/               
//	readMsgBlock(); //Ryan: Need to add code

	/*Ryan
	   3.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
	*/
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
	
	/*Ryan
		 4.	If training is required at another frequency, repeat the operations starting at step (E).
	*/
}	

void phyinit_D_Load_2D_IMEM(void)
{	
	//Ryan
	//##############################################################
	//
	// (D) Load the 2D IMEM image
	// 
	// This function loads the training firmware IMEM image into the SRAM.
	// See PhyInit App Note for detailed description and function usage
	// 
	//##############################################################

	/*Ryan 
				 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
					  This allows the memory controller unrestricted access to the configuration CSRs. 
	*/
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);

	/*Ryan Reading input file: dwc_lpddr4_multiphy_v2_tsmc28hpcp18/1.40a/firmware/A-2017.05/lpddr4/lpddr4_2d_pmu_train_imem.bin
				 offset 0x50000 size 0x4000
	*/
	//ddr_load_train_code(FW_2D_IMAGE); //Ryan: Need to check
	
	/*Ryan 
				 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
			  This allows the firmware unrestricted access to the configuration CSRs.
	*/ 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
}	

void phyinit_F_Load_2D_DMEM(void)
{	
	//Ryan
	//##############################################################
	//
	// Setp (F) Load the 2D DMEM image and write the 1D Message Block parameters for the training firmware 
	// 
	// The procedure is as follows: 
	// 
	//##############################################################

	/*Ryan 
				 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
					  This allows the memory controller unrestricted access to the configuration CSRs. 
	*/
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);

	/*Ryan Reading input file: dwc_lpddr4_multiphy_v2_tsmc28hpcp18/1.40a/firmware/A-2017.05/lpddr4/lpddr4_2d_pmu_train_dmem.bin
				 offset 0x54000 size 0x29a
	*/
	//ddr_load_train_code(FW_2D_IMAGE); //Ryan: Need to check
	
	/*Ryan 
				 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
			  This allows the firmware unrestricted access to the configuration CSRs.
	*/ 
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);
}

void phyinit_I_Load_PIE(void)
{	
	//Ryan
	//##############################################################
	//
	// Step (I) Load PHY Init Engine Image 
	// 
	// Load the PHY Initialization Engine memory with the provided initialization sequence.
	// 
	// <Note: For LPDDR3/LPDDR4, this sequence will include the necessary retraining code.>
	// 
	//##############################################################
	
	/*Ryan
		 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
	*/              
	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x0);  

	/*Ryan
		Programming PIE Production Code
	*/
	WritePIECode(); //Ryan: Need to add code

	ddrphy_apb_wr(DDRPHY_REG(0xd0000), 0x1);  
}

void dram_phy_cfg(void)
{

  phyinit_C_initPhyConfig();
  if(!SKIP_TRAIN)
  {
	phyinit_D_Load_1D_IMEM();
	// phyinit_E_setDFIClk();
	// phyinit_F_Load_1D_DMEM();
	phyinit_F_Write_Message_Block();
	phyinit_G_execFW();
	phyinit_H_Read_Message_Block();
	if(TRAIN_2D)
	{
	  phyinit_D_Load_2D_IMEM();
	  phyinit_F_Load_2D_DMEM();
	  phyinit_G_execFW();
	  phyinit_H_Read_Message_Block();
	}
  }
  phyinit_I_Load_PIE();

	return;
}

void ddr_init()
{
	/* Set the DDRC clock frequency, according to "JiaXing config PLL sequence 0.3.pdf" */
	printf("SMU: clock frequency config\n");
	// set_ddrc_freq(800);

	//init sequence
	printf("DDRC: init start\n");
	reg32_write(0x190d208,0x10000000);   //Ryan: Assert DDR_PWROK high
	reg32_write(0x190d10c,0x20000000);   //Ryan: Release DDR_APB_SRSTN

	/* Configure DRAM controller registers */
	dram_controller_cfg();
	

	reg32_write(0x0190d10c,0xFFFFFFFF);  //Ryan: Release DDR_CORE_SRSTN and DDR_SYS_SRSTN also


	reg32_write(DDRC_BASE_REG(0x304), 0x00000000);	//Ryan: Has been clear in dram_controller_cfg	      
	reg32_write(DDRC_BASE_REG(0x30), 0x000000a8);   /*Ryan: [7]: 1 (need to confirm)
															[6]: 1 (need to confirm)
															[3]: 1 (need to confirm)
															[0]: 0 (leave Self Refresh Mode)
													*/        
	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x33f) != 0x223) //Ryan: Check Status
		;                                                       //      [3:0]: 11 (Self Refresh mode)
																//      [5:4]: 10 (SR-Powerdone)
																//      [9:8]: 10 (Self refresh pwoer down)

	printf("DDRC: under self-refresh mode\n");

	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);  //Ryan: Program register to 0 to enable quasi-dynamic programming         
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000010);  //Ryan: DFIMISC  [4]    : 1 (ctl_idle_en)
													//               [12:8] : 0 (indicates the operating frequency of the system)


	/* LPDDR4 PHY config and training */
	printf("DDR PHY: init start\n");
	dram_phy_cfg();
	
	reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000030);           
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);           

	while ((reg32_read(DDRC_BASE_REG(0x1BC)) & 0x1) == 0x0)  
		;
	printf("DDR: DFISTAT.dfi_init_complete\n");

	reg32_write(DDRC_BASE_REG(0x320), 0x00000000);           
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000050);           
	reg32_write(DDRC_BASE_REG(0x1B0), 0x00000051);           
	reg32_write(DDRC_BASE_REG(0x30), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x320), 0x00000001);  	         
	while ((reg32_read(DDRC_BASE_REG(0x324)) & 0x1) == 0x0)  
		;
	while ((reg32_read(DDRC_BASE_REG(0x4)) & 0x3) != 0x1)    
		;
  printf("DDR: return to normal state\n");

	reg32_write(DDRC_BASE_REG(0x30), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x30), 0x00000000);            
	reg32_write(DDRC_BASE_REG(0x490), 0x00000001);           
	reg32_write(DDRC_BASE_REG(0x540), 0x00000001); 
	//reg32_write(DDRC_BASE_REG(0x60), 0x00000000);            
	
	printf("DDR: init full done!\n");

	reg32_write(0x80000000, 0x5a5aa5a5);
	printf("read ddr 0x%8x", reg32_read(0x80000000));

	return;
}


