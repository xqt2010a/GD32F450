#ifndef __JX_SPI_H__
#define __JX_SPI_H__

#include "dwc_spi.h"

typedef enum
{
    SPI0 = 0,
    SPI1 = 1,
    SPI2 = 2,
    SPI3 = 3,
} SPIn;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
void SPI_Init(SPI_InitTypeDef * spi);
/* Data transfers functions ***************************************************/
int SPI_Read_ID(void);
void SPIM1_Write_Read(uint8_t * wBuf, uint32_t wLen, uint8_t * rBuf, uint32_t rLen);

#ifdef __cplusplus
}
#endif


#endif  /* __JX_SPI_H__ */