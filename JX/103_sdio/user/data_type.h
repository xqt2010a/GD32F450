/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/
#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int		BOOL;
typedef unsigned int		bool;

#define debug   printf
#define info    printf

#define TRUE				(1)
#define FALSE				(0)
#define false				(0)
#define true				(1)

#define BIT(nr)	(1UL << (nr))
#define ROUND(a,b)	(((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define LOG2(x) (((x & 0xaaaaaaaa) ? 1 : 0) + ((x & 0xcccccccc) ? 2 : 0) + \
		 ((x & 0xf0f0f0f0) ? 4 : 0) + ((x & 0xff00ff00) ? 8 : 0) + \
		 ((x & 0xffff0000) ? 16 : 0))

#define __swab32(x) \
	((unsigned int)( \
		(((unsigned int)(x) & (unsigned int)0x000000ffUL) << 24) | \
		(((unsigned int)(x) & (unsigned int)0x0000ff00UL) <<  8) | \
		(((unsigned int)(x) & (unsigned int)0x00ff0000UL) >>  8) | \
		(((unsigned int)(x) & (unsigned int)0xff000000UL) >> 24) ))
#define __be32_to_cpu(x) __swab32(x)
#define cpu_to_le16(x)		(x)
#define le16_to_cpu(x)		(x)
#define le32_to_cpu(x)		(x)

#define dma_virt_to_phy(x)	(x)
	
/* memory operation definition*/
#define REG32(addr)   		(*((volatile unsigned int *)(addr)))

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define wfi(option) __asm__ __volatile__ ("wfi " #option : : : "memory")
#define wfe(option) __asm__ __volatile__ ("wfe " #option : : : "memory")
#define bkpt() __asm__ __volatile__ ("bkpt " : : : "memory")

#define __raw_writew __raw_writew
static inline void __raw_writew(unsigned short val, volatile void *addr)
{
//	__asm__ volatile("strh %1, %0"
//		     : "+Q" (*(volatile unsigned short *)addr)
//		     : "r" (val));
    (*(volatile unsigned short *)addr) = val;
}

#define __raw_readw __raw_readw
static inline unsigned short __raw_readw(const volatile void *addr)
{
	unsigned short val;
//	__asm__ volatile("ldrh %1, %0"
//		     : "+Q" (*(volatile unsigned short *)addr),
//		       "=r" (val));
    val = (*(volatile unsigned short *)addr);
	return val;
}

#define __raw_writeb __raw_writeb
static inline void __raw_writeb(unsigned char val, volatile void *addr)
{
//	__asm__ volatile("strb %1, %0"
//		     : "+Qo" (*(volatile unsigned char *)addr)
//		     : "r" (val));
    (*(volatile unsigned char *)addr) = val;
}

#define __raw_writel __raw_writel
static inline void __raw_writel(unsigned int val, volatile void *addr)
{
//	asm volatile("str %1, %0"
//		     : "+Qo" (*(volatile unsigned int *)addr)
//		     : "r" (val));
    (*(volatile unsigned int *)addr) = val;
}

#define __raw_readb __raw_readb
static inline unsigned char __raw_readb(const volatile void *addr)
{
	unsigned char val;
//	__asm__ volatile("ldrb %1, %0"
//		     : "+Qo" (*(volatile unsigned char *)addr),
//		       "=r" (val));
    val = (*(volatile unsigned char *)addr);
	return val;
}

#define __raw_readl __raw_readl
static inline unsigned int __raw_readl(const volatile void *addr)
{
	unsigned int val;
//	__asm__ volatile("ldr %1, %0"
//		     : "+Qo" (*(volatile unsigned int *)addr),
//		       "=r" (val));
    val = (*(volatile unsigned int *)addr);
	return val;
}

#define isb(option) 	__asm__ __volatile__ ("isb " #option : : : "memory")
#define dsb(option) 	__asm__ __volatile__ ("dsb " #option : : : "memory")
#define dmb(option) 	__asm__ __volatile__ ("dmb " #option : : : "memory")

#define mb()			do { dsb(); } while (0)
#define rmb()			dsb()
#define wmb()			do { dsb(st); } while (0)
#define dma_rmb()		dmb(osh)
#define dma_wmb()		dmb(oshst)

#define __iormb()		rmb()
#define __iowmb()		wmb()


#define readb_relaxed(c) ({ unsigned char  __r = __raw_readb(c); __r; })
#define readw_relaxed(c) ({ unsigned short __r = (unsigned short) __raw_readw(c); __r; })
#define readl_relaxed(c) ({ unsigned int __r = (unsigned int) __raw_readl(c); __r; })

#define writeb_relaxed(v,c)	__raw_writeb(v,c)
#define writew_relaxed(v,c)	__raw_writew((unsigned short)(v),c)
#define writel_relaxed(v,c)	__raw_writel((unsigned int)(v),c)

#define readb(c)		({ unsigned char  __v = readb_relaxed(c); __iormb(); __v; })
#define readw(c)		({ unsigned short __v = readw_relaxed(c); __iormb(); __v; })
#define readl(c)		({ unsigned int __v = readl_relaxed(c); __iormb(); __v; })

#define writeb(v,c)		({ __iowmb(); writeb_relaxed(v,c); })
#define writew(v,c)		({ __iowmb(); writew_relaxed(v,c); })
#define writel(v,c)		({ __iowmb(); writel_relaxed(v,c); })

#endif

