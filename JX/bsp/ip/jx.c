#include "jx.h"

inline void register_write(uint32_t addr, uint8_t mask_bit, uint8_t start_bit, uint32_t value)
{
    uint32_t temp,data;
    temp = JX_WR4(addr);
    data = (temp & (~((((uint32_t)1<<mask_bit)-1)<<start_bit)))|(value<<start_bit);
    JX_WR4(addr) = data;
}

inline uint32_t register_read(uint32_t addr, uint8_t mask_bit, uint8_t start_bit)
{
    uint32_t temp,data;
    temp = JX_WR4(addr);
    data = (temp & ((((uint32_t)1<<mask_bit)-1)<<start_bit))>>start_bit;
    return data;
}