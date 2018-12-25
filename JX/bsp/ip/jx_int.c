#include "jx.h"
#include "jx_int.h"
#include <intrinsics.h>


#define INT_NUM     229


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

void gic_reg_op(int base, int step, int val, unsigned int irqnum, int op)
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
    GICC_CTLR = 1;     //Processor Interface Control Register
    GICD_CTRL = 0x3;   //enable non-secure and secure
}

void IRQ_SetNum(unsigned int irqnum)
{
//    int op = 1;

    /* DETECT 0x03: high level trigger*/
    //gic_reg_op(INT_ICDICFR, 0x2, 0x3, irqnum, op);
    /* Priority */
    //gic_reg_op(INT_ICDIPR, 0x8, 0xA0, irqnum, op);
    /* Target CPU */
    //gic_reg_op(INT_ICDIPTR, 0x8, 0x01, irqnum, op);
    /* Enable Set */
    //gic_reg_op(INT_ICDISER, 0x1, 0x1, irqnum, op);
    
    /* DETECT 0x03: high level trigger*/
//    gic_reg_op(SYS_GIC_BASE+0x1C00, 0x2, 0x3, irqnum, op);
//    gic_reg_op(SYS_GIC_BASE+0x1400, 0x8, 0xA0, irqnum, op);
//    gic_reg_op(SYS_GIC_BASE+0x1800, 0x8, 0x01, irqnum, op);
//    gic_reg_op(SYS_GIC_BASE+0x1100, 0x1, 0x1, irqnum, op);
    // Interrupt Configuration Registers, DETECT 0x03: high level trigger
    register_write((uint32_t)&GICD_ICFGR(irqnum/32), 2, irqnum%32, 0x3);
    // Priority, 10
    register_write((uint32_t)&GICD_IPRIORITYR(irqnum/4), 8, (irqnum%4)*8, 0xA0);
    // Target CPU, 1=cpu0 2=cpu1 4=cpu2 8=cpu3
    register_write((uint32_t)&GICD_ITARGETSR(irqnum/4), 8, (irqnum%4)*8, 0x1);
    // Interrupt Set-Enable Registers, 
    register_write((uint32_t)&GICD_ISENABLER(irqnum/32), 1, irqnum%32, 0x1);
}

void IRQ_Init(void)
{
    unsigned int i;
    GICD_CTRL = 0;     //disable seclect cpu interface
    GICC_CTLR = 0;     //disable cpu interrupt
    
//    for(i = 0; i< 1020; i++){
//        //gic_reg_op(INT_ICDISER, 0x1, 0x1, i, 0);
//        gic_reg_op(SYS_GIC_BASE+0x1100, 0x1, 0x1, i, 0);
//    }
    for(i=32; i<INT_NUM; i++){
        register_write((uint32_t)&GICD_ISENABLER(i/32), 1, i%32, 0);
    }
    //GICD_CTRL = 0x3;   //enable non-secure and secure
    GICC_PMR = 0xff; //Priority Mask Register, 0xff handle all interrupt
    //IRQ_SetEnable();
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
    id = GICC_IAR;
    id = id & 0x3FF;
    return id;
}

void IRQ_Handler(void)
{
    uint32_t id;
    id = IRQ_GetID();
    IRQ_HandleFunc[id]();
    GICC_EOIR = id;   //end of interrupt register
    /* Ensure the write takes before re-enabling interrupts. */
	__DSB();
	__ISB();
    __enable_irq();
}