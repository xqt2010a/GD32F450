/*
 * iic.h
 *
 *  Created on: 2018Äê6ÔÂ26ÈÕ
 *      Author: cnan
 */

#ifndef __I2C_H__
#define __I2C_H__

#include "dwc_i2c.h"

typedef enum
{
	I2C0	= 0,
	I2C1	= 1,
	I2C2	= 2,
	I2C3	= 3,
	I2C4	= 4,
	I2C5	= 5,
	I2C6	= 6,
	I2C7	= 7,
	I2C8	= 8,
	I2C9	= 9,
	I2C10	= 10,
	I2C11	= 11,
	I2C12	= 12,
	I2C13	= 13,
	I2C14	= 14,
	I2C15	= 15
}I2C_PORT;

void i2c_init(I2C_InitTypeDef *i2c);

#endif /* __I2C_H__ */
