#ifndef __JX_H__
#define __JX_H__

#include "stdint.h"

#define JX_WR4(x)    (*(volatile unsigned int *)(x))

void register_write(uint32_t addr, uint8_t mask_bit, uint8_t start_bit, uint32_t value);
uint32_t register_read(uint32_t addr, uint8_t mask_bit, uint8_t start_bit);

#endif  /* __JX_H__ */