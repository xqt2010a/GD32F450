/**********************************************************************************************************/
//
//  Filename :          uart_reg.h
//
//  COPYRIGHT, 2018
//  The contents of this file is the sole proprietary property of, confidential
//  to, and copyrighted by Brite Company.  It may not be copied or reproduced
//  in any manner or form without the prior written consent of Brite Company.
//
//  Original Author:    xxx
//  Date created:       02, 07, 2018
//
//  Description:
//      define uart registers
//
//  Revision:
//  Date       		Author      	Description
//  02/07/16    	xxx   		    Original
//
/*************************************************************************************************************/
#ifndef _UART_H_
#define _UART_H_

#include "stdio.h"

#define UART_CTRL_BASE_ADDR 	(0x3ff25000)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// register layout
#define UART_RBR_REG		(UART_CTRL_BASE_ADDR + 0x00)	// R, 	RX Buffer Register
#define UART_THR_REG		(UART_CTRL_BASE_ADDR + 0x00)	// W, 	TX Holding Register
#define UART_DLL_REG		(UART_CTRL_BASE_ADDR + 0x00)	// R/W, 	Divisor Latch (low)

#define UART_DLH_REG		(UART_CTRL_BASE_ADDR + 0x04)	// R/W,	Divisor Latch (high)
#define UART_IER_REG		(UART_CTRL_BASE_ADDR + 0x04)	// R/W,	Interrupt Enable Register

#define UART_IIR_REG		(UART_CTRL_BASE_ADDR + 0x08)	// R,		Interrupt Identification
#define UART_FCR_REG		(UART_CTRL_BASE_ADDR + 0x08)	// W,	FIFO Control Register

#define UART_LCR_REG		(UART_CTRL_BASE_ADDR + 0x0C)	// R/W,	Line Control Register
#define UART_MCR_REG		(UART_CTRL_BASE_ADDR + 0x10)	// R/W,	Modem Control Register
#define UART_LSR_REG		(UART_CTRL_BASE_ADDR + 0x14)	// R,		Line Status Register
#define UART_MSR_REG		(UART_CTRL_BASE_ADDR + 0x18)	// R,		Modem Status Register
#define UART_SCR_REG		(UART_CTRL_BASE_ADDR + 0x1C)	// R/W,	Scratch Register

#define UART_LPDLL_REG		(UART_CTRL_BASE_ADDR + 0x20)	// R/W,	Low Power Divisor Latch (Low) Register
#define UART_LPDLH_REG		(UART_CTRL_BASE_ADDR + 0x24)	// R/W,	Low Power Divisor Latch (Low) Register

/*************************************************************************************************************/
// define  interrupt identification flag
//FIFOs Enabled
typedef enum
{
	UART_FIFO_DIS	= 0,
	UART_FIFO_EN	= 3
} E_UART_FIFO_CTRL;

// Interrupt ID
typedef enum
{
	UART_INT_ID_MODEM_ST		= 0,
	UART_INT_ID_NO_INT_PENDING	= 1,
	UART_INT_ID_THR_EMPTY		= 2,
	UART_INT_ID_RCV_DATA_AVAI	= 4,
	UART_INT_ID_RCV_LINE_ST		= 6,
	UART_INT_ID_BUSY_DECT		= 7,
	UART_INT_ID_CH_TIMEOUT		= 0x0C
} E_UART_INT_ID;

// offset: 0x08,  FIFO Control Register
typedef enum
{
	FCR_FIFO_EN_SHIFT 	= 0,
	FCR_RFIFOR_SHIFT 	= 1,
	FCR_XFIFOR_SHIFT 	= 2,
	FCR_DMA_MODE_SHIFT 	= 3,
	FCR_TET_SHIFT 	 	= 4,
	FCR_RT_SHIFT 	 	= 6
}E_UART_FCR_SHIFT;

// [7:6], RCVR Trigger(or RT)
typedef enum
{
	RT_FIFO_1_CHARA   			= 0x0,
	RT_FIFO_1_4_FULL  			= 0x1,
	RT_FIFO_1_2_FULL			= 0x2,
	RT_FIFO_2_LESS_THAN_FULL  	= 0x3
} E_UART_RT;

// [5:4], TX Empty Trigger (or TET)
typedef enum
{
	TET_FIFO_EMPTY   	= 0x0,
	TET_FIFO_2CHARA  	= 0x1,
	TET_FIFO_1_4_FULL  	= 0x2,
	TET_FIFO_1_2_FULL  	= 0x3
} E_UART_TET;

#define UART_FCR_VAL(rfifor, xfifor)\
			(((RT_FIFO_1_CHARA) << FCR_RT_SHIFT)|\
			((TET_FIFO_1_4_FULL) << FCR_TET_SHIFT)|\
			((0) << FCR_DMA_MODE_SHIFT)|\
			((xfifor) << FCR_XFIFOR_SHIFT)|\
			((rfifor) << FCR_RFIFOR_SHIFT)|\
			((1) << FCR_FIFO_EN_SHIFT))
// LCR

// Divisor Latch Access Bit
typedef enum
{
	UART_DLAB_DIS   = 0x0,
	UART_DLAB_EN    = 0x1
} E_UART_DLAB;

// define transmitted and received character size
typedef enum
{
	UART_DATABITS_5    = 0x0,
	UART_DATABITS_6    = 0x1,
	UART_DATABITS_7    = 0x2,
	UART_DATABITS_8    = 0x3
} E_UART_DataBits;

// stop bits
typedef enum
{
	UART_STOPBITS_1		= 0x0,
	UART_STOPBITS_1_5	= 0x1,
	UART_STOPBITS_2		= 0x1
} E_UART_StopBits;

// PARITY Setting
typedef enum
{
	UART_PARITY_NONE   = 0x0,
	UART_PARITY_ODD    = 0x2,
	UART_PARITY_EVEN   = 0x3
} E_PARITY_Set;

// FIFO Select
typedef enum
{
	UART_FIFOBYTES_1   = 0x0,
	UART_FIFOBYTES_4   = 0x1,
	UART_FIFOBYTES_8   = 0x2,
	UART_FIFOBYTES_14  = 0x3
} E_FIFO_Bytes;

// baud rate setting
typedef enum
{
	UART_BD_300		 = 300,
	UART_BD_4800	 = 4800,
	UART_BD_7200	 = 7200,
	UART_BD_9600	 = 9600,
	UART_BD_14400	 = 14400,
	UART_BD_19200	 = 19200,
	UART_BD_38400	 = 38400,
	UART_BD_57600	 = 57600,
	UART_BD_115200	 = 115200,
	UART_BD_230400   = 230400,
	UART_BD_460800   = 460800,
	UART_BD_921600   = 921600,
	UART_BD_1843200  = 1843200,
	UART_BD_3000000  = 3000000,
	UART_BD_3686400  = 3686400
} E_BD_Set;

// LSR-line status register(read only)
typedef enum
{
	LSR_FIFOERR	= (1<<7),	// RX Data Error in FIFO
	LSR_TEMT	= (1<<6),	// Transmitter Empty
	LSR_THRE	= (1<<5),	// TX Holding Register Empty
	LSR_BI		= (1<<4),	// Break Interrupt
	LSR_FE		= (1<<3),	// Framing Error
	LSR_PE		= (1<<2),	// Parity Error
	LSR_OE		= (1<<1),	// Overrun Error
	LSR_RDR		= (1<<0)	// Data Ready
} E_UART_LSR;

#define MCR_DTR         (1<<0)
#define MCR_RTS         (1<<1)
#define MCR_AFCE        (1<<5)

#define UART_FLOWCTRL_NONE      (MCR_DTR  | MCR_RTS)
#define UART_FLOWCTRL_HW        (MCR_AFCE | MCR_RTS)



 


void uart_init(unsigned char parity, unsigned char stop_bits, unsigned char data_bits, unsigned int bd);
#endif

