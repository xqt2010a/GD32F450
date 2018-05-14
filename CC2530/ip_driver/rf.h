#ifndef __RF_H__
#define __RF_H__

#include "ioCC2530.h"

void Rf_Init(void);
void Rf_Send( unsigned char *pbuf , unsigned int len);
unsigned int Rf_Receive_Isr(unsigned char * rf_rx_buf);

#endif  /* __RF_H__ */