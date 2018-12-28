#include "gic.h"

void gic_enable(void)
{
    GICC_CTLR = 1;     //Processor Interface Control Register
    GICD_CTRL = 0x3;   //enable non-secure and secure
}

void gic_set_num(unsigned int irqnum)
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

void gic_init(void)
{
    unsigned int i;
    GICD_CTRL = 0;     //disable seclect cpu interface
    GICC_CTLR = 0;     //disable cpu interrupt
    
    for(i=32; i<INT_NUM; i++){
        register_write((uint32_t)&GICD_ISENABLER(i/32), 1, i%32, 0);
    }
    GICC_PMR = 0xff; //Priority Mask Register, 0xff handle all interrupt
}

uint32_t gic_id(void)
{
    return GICC_IAR & 0x3FF;
}

void gic_clear_id(uint32_t id)
{
    register_write((uint32_t)&GICD_ICPENDR(id/32), 1, id%32, 0x1);
    GICC_EOIR = GICC_EOIR & (~0x3FF) | id;   //end of interrupt register
}

