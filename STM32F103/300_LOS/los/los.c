#include "los.h"

#define os_HighestPrio  1

uint32_t os_Timers[config_MAX_TASK];
uint32_t os_Event[config_MAX_TASK];
