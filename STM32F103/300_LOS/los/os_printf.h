#ifndef __OS_PRINTF_H__
#define __OS_PRINTF_H__

#include "SEGGER_RTT.h"

#define PRT(...) SEGGER_RTT_printf(0, __VA_ARGS__)

#endif  /* __OS_PRINTF_H__ */