#include "jx.h"
#include "jx_int.h"
#include <intrinsics.h>


#define INT_NUM     256


static void (*IRQ_HandleFunc[INT_NUM])(void);

static unsigned int step_mask(int step)
{
	int i;
	unsigned int mask=0;

	/* We assume that step is between 1 and 31. No check here.*/
	for(i = 0;i < step; i++){
		mask |= 0x1 << i;
	}
	return mask;
}

static void gic_reg_op(int base, int step, int val, unsigned int irqnum, int op)
{
	unsigned int addr, offset, tmpval;
	addr = base + (irqnum*step/32)*4;
	offset = (irqnum*step)%32;

	tmpval = JX_WR4(addr);
	if(op) {
		tmpval |= (val&step_mask(step)) << offset;
	} else {
		tmpval &= ~(step_mask(step) << offset);
	}

	JX_WR4(addr) = tmpval;
	return ;
}

void IRQ_SetEnable(void)
{
    JX_WR4(INT_ICPICR) = 1;     //Processor Interface Control Register
    JX_WR4(INT_ICDDCR) = 0x3;   //enable non-secure and secure
}

void IRQ_SetNum(unsigned int irqnum)
{
    int op = 1;

	/* DETECT 0x03: high level trigger*/
	gic_reg_op(INT_ICDICFR, 0x2, 0x3, irqnum, op);
	/* Priority */
	gic_reg_op(INT_ICDIPR, 0x8, 0xA0, irqnum, op);
	/* Target CPU */
	gic_reg_op(INT_ICDIPTR, 0x8, 0x01, irqnum, op);
	/* Enable Set */
	gic_reg_op(INT_ICDISER, 0x1, 0x1, irqnum, op);
}

void IRQ_Init(void)
{
    unsigned int i;
    JX_WR4(INT_ICDDCR) = 0;     //clear Distributor Control Register
    JX_WR4(INT_ICPICR) = 0;     //clear Processor Interface Control Register
//    for(i=0; i<8; i++){
//        JX_WR4(INT_ICDISER) = 0;
//    }
    for(i = 0; i< 1020; i++){
		gic_reg_op(INT_ICDISER, 0x1, 0x1, i, 0);
	}
    JX_WR4(INT_ICDDCR) = 0x3;   //enable non-secure and secure
    JX_WR4(INT_ICCIPMR) = 0xff; //Priority Mask Register
    IRQ_SetEnable();
}

int IRQ_Register(unsigned int irqnum, void f(void))
{
    IRQ_HandleFunc[irqnum] = f;
    IRQ_SetNum(irqnum);
    return 0;
}

uint32_t IRQ_GetID(void)
{
    uint32_t id;
    id = JX_WR4(INT_ICCIAR);
    id = id & 0x3FF;
    return id;
}

void IRQ_Handler(void)
{
    uint32_t id;
    id = IRQ_GetID();
    IRQ_HandleFunc[id]();
    JX_WR4(INT_ICCEOIR) = id;   //end of interrupt register
    /* Ensure the write takes before re-enabling interrupts. */
	__DSB();
	__ISB();
    __enable_irq();
}