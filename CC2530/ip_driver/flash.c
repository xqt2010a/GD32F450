#include "ioCC2530.h"
#include "flash.h"

unsigned char Mac_Addr[8];

void Mac_Addr_Get(unsigned char * buf)
{
  	unsigned char i;
	for(i=0; i<8; i++){
		buf[i] = 0;
	}
	unsigned char * mac_addr_temp = (unsigned char *)(P_INFOPAGE+0x0C);
	for(i=0; i<8; i++){
		buf[i] = mac_addr_temp[i];
	}
}
/************************************************************************
* 功能： 擦除FLASH中的第page页
* 参数： page = 0-127(Z-Stack协议栈规定用户可使用范围page=121-126)
************************************************************************/
void Flash_Erase(unsigned char page)
{
	FADDRH = page<<1;
	FCTL |= 0x01;  //启动页面擦除
	while(FCTL & 0x80);  //等待擦除完成
}

/************************************************************************
* 功能： 将buf中的length个字节写入FLASH第page页offset开始的位置
* 参数： page=0-127，offset和length必须是4的整倍数,offset<<2K
* 说明： 可去掉Project->Options->Texas?Instruments->Download中的Erase flash选项
************************************************************************/
unsigned char DmaDesc[8]={0,0,0x62,0x73,0,0,0x12,0x42};  //定义DMA描述符
//addr = (offset + pg * 2KB)/4
void Flash_Write(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length)
{
	FADDRH = (page << 1)|((offset >> 10) & 1); //计算FLASH地址
	FADDRL = (offset >> 2) & 0xFF;
	DmaDesc[0] = (unsigned int)buf>>8;  //设置数据地址
	DmaDesc[1] = (unsigned int)buf&0xFF;
	DmaDesc[4] = (length>>8)&0x1F;  //设置字节数
	DmaDesc[5] = length & 0xFC;
	
	DMA0CFGH = (unsigned int)DmaDesc >> 8;  //设置DMA通道0描述符地址
	DMA0CFGL = (unsigned int)DmaDesc & 0xFF;
	DMAIRQ = 0x01;  //激活DMA通道0
	DMAARM = 0x01;  //使DMA通道0进入工作状态
	FCTL |= 0x02;  //开始DMA写入操作
	while(FCTL & 0x80);  //等待写入完成
}

/************************************************************************
* 功能： 将FLASH第page页offset开始的length个字节读入buf中
* 参数： page=0-127，offset和length必须是4的整倍数
************************************************************************/
void Flash_Read(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length)
{
	unsigned char *pData = (unsigned char *)(offset + (page & 0xF) * 2048 + 0x8000);
	unsigned char memctr = MEMCTR;   //暂存当前存储器映射状态
	MEMCTR = (MEMCTR & 0xF8)|((page>>4)&7);  //将被读Flash映射到XDATA
	while(length--)
	  	*buf++ = *pData++;  //读取数据
	MEMCTR = memctr;	//回复存储器映射状态
}


