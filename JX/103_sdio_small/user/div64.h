/*
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 * Based on former asm-ppc/div64.h and asm-m68knommu/div64.h
 *
 * The semantics of do_div() are:
 *
 * unsigned int do_div(unsigned long long *n, unsigned int base)
 * {
 *	unsigned int remainder = *n % base;
 *	*n = *n / base;
 *	return remainder;
 * }
 *
 * NOTE: macro parameter n is evaluated multiple times,
 *			 beware of side effects!
 */

#ifndef _ASM_GENERIC_DIV64_H
#define _ASM_GENERIC_DIV64_H


#define typeof __typeof__

extern unsigned int __div64_32( unsigned long long* dividend, unsigned int divisor );

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({				\
	unsigned int __base = (base);			\
	unsigned int __rem;					\
	if (((n) >> 32) == 0) {			\
		__rem = (unsigned int)(n) % __base;		\
		(n) = (unsigned int)(n) / __base;		\
	} else						\
		__rem = __div64_32(&(n), __base);	\
	__rem;						\
 })

unsigned long long cb_lldiv(unsigned long long dividend, unsigned int divisor);

#endif /* _ASM_GENERIC_DIV64_H */
