#ifndef __FLASH_H__
#define __FLASH_H__

extern unsigned char Mac_Addr[8];
extern void Mac_Addr_Get(unsigned char * buf);
void Flash_Erase(unsigned char page);
void Flash_Write(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length);
void Flash_Read(unsigned char page, unsigned int offset, unsigned char *buf, unsigned int length);

#endif  /* __FLASH_H__ */