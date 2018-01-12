#include "los_mem.h"

void * os_Malloc(uint32_t bytes)
{
    void *addr;
    addr = malloc(bypes);
    return addr;
}

void os_Free(void * addr)
{
    free(addr);
}