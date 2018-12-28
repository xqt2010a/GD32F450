#include "jx_int.h"

static void (*IRQ_HandleFunc[INT_NUM])(void);

void int_init(void)
{
    gic_init();
}

void int_enable(void)
{
    gic_enable();
}

void int_register(unsigned int irqnum, void f(void))
{
    IRQ_HandleFunc[irqnum] = f;
    gic_set_num(irqnum);
}

void INT_Handler(void)
{
    uint32_t id;
    id = gic_id();
    IRQ_HandleFunc[id]();
    gic_clear_id(id);
}