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
* ���ܣ� ����FLASH�еĵ�pageҳ
* ������ page = 0-127(Z-StackЭ��ջ�涨�û���ʹ�÷�Χpage=121-126)
************************************************************************/
void Flash_Erase(unsigned char page)
{
	FADDRH = page<<1;
	FCTL |= 0x01;  //����ҳ�����
	while(FCTL & 0x80);  //�ȴ��������
}

/************************************************************************
* ���ܣ� ��buf�е�length���ֽ�д��FLASH��pageҳoffset��ʼ��λ��
* ������ page=0-127��offset��length������4��������,offset<<2K
* ˵���� ��ȥ��Project->Options->Texas?Instruments->Download�е�Erase flashѡ��
************************************************************************/
unsigned char DmaDesc[8]={0,0,0x62,0x73,0,0,0x12,0x42};  //����DMA������
//addr = (offset + pg * 2KB)/4
void Flash_Write(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length)
{
	FADDRH = (page << 1)|((offset >> 10) & 1); //����FLASH��ַ
	FADDRL = (offset >> 2) & 0xFF;
	DmaDesc[0] = (unsigned int)buf>>8;  //�������ݵ�ַ
	DmaDesc[1] = (unsigned int)buf&0xFF;
	DmaDesc[4] = (length>>8)&0x1F;  //�����ֽ���
	DmaDesc[5] = length & 0xFC;
	
	DMA0CFGH = (unsigned int)DmaDesc >> 8;  //����DMAͨ��0��������ַ
	DMA0CFGL = (unsigned int)DmaDesc & 0xFF;
	DMAIRQ = 0x01;  //����DMAͨ��0
	DMAARM = 0x01;  //ʹDMAͨ��0���빤��״̬
	FCTL |= 0x02;  //��ʼDMAд�����
	while(FCTL & 0x80);  //�ȴ�д�����
}

/************************************************************************
* ���ܣ� ��FLASH��pageҳoffset��ʼ��length���ֽڶ���buf��
* ������ page=0-127��offset��length������4��������
************************************************************************/
void Flash_Read(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length)
{
	unsigned char *pData = (unsigned char *)(offset + (page & 0xF) * 2048 + 0x8000);
	unsigned char memctr = MEMCTR;   //�ݴ浱ǰ�洢��ӳ��״̬
	MEMCTR = (MEMCTR & 0xF8)|((page>>4)&7);  //������Flashӳ�䵽XDATA
	while(length--)
	  	*buf++ = *pData++;  //��ȡ����
	MEMCTR = memctr;	//�ظ��洢��ӳ��״̬
}


