/*
 * I2C.c
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: cnan
 */
#include "jx_i2c.h"

void i2c_init(I2C_InitTypeDef *i2c)
{
    JX_WR4(0x3fe0a004) = 0x0;   //i2c module enable
    dwc_i2c_init(i2c);
}

//uint16_t read_lm75a(unsigned char ch)
//{
//    uint16_t r_value, i, buf[4];
//    I2C_DATA_CMD(ch) = (0x00&0x0FF)|(1<<9);;    //read temp
//    I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8);
//    I2C_DATA_CMD(ch) = DUMMY_WRITE|(1<<8)|(1<<9);  //read & stop
//    i=0;
//	while(i<2){
//	  	while((I2C_STATUS(ch)&(1<<3))){
//			buf[i] = (unsigned char)(I2C_DATA_CMD(ch) & 0x0FF);
//			i++;
//		}
//	}
//    r_value = ((buf[1]&0x7F)<<3)|(buf[2]>>5);
//    return r_value;
//}