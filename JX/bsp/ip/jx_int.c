#include "jx.h"
#include "jx_int.h"
#include <intrinsics.h>

#define INT_NUM     229

static void (*IRQ_HandleFunc[INT_NUM])(void);

void IRQ_SetEnable(void)
{
    GICC_CTLR = 1;     //Processor Interface Control Register
    GICD_CTRL = 0x3;   //enable non-secure and secure
}

void IRQ_SetNum(unsigned int irqnum)
{
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
    
    for(i=32; i<INT_NUM; i++){
        register_write((uint32_t)&GICD_ISENABLER(i/32), 1, i%32, 0);
    }
    GICC_PMR = 0xff; //Priority Mask Register, 0xff handle all interrupt
}

int IRQ_Register(unsigned int irqnum, void f(void))
{
    IRQ_HandleFunc[irqnum] = f;
    IRQ_SetNum(irqnum);
    return 0;
}

uint32_t IRQ_GetID(void)
{
    return GICC_IAR & 0x3FF;
}



void INT_Handler(void)
{
    uint32_t id, value;
    value = GICC_IAR;
    id = value & 0x3FF;
    IRQ_HandleFunc[id]();
    register_write((uint32_t)&GICD_ICPENDR(id/32), 1, id%32, 0x1);
    GICC_EOIR = value;//GICC_EOIR & (~0x3FF) | id;   //end of interrupt register
    /* Ensure the write takes before re-enabling interrupts. */
	//__DSB();
	//__ISB();
    //__enable_irq();
    //return;
}