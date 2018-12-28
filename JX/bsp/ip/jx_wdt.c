#include "jx_wdt.h"

void wdt_init(WDT_InitTypeDef * wdt)
{
    dwc_wdt_set(wdt);
    dwc_wdt_mode(wdt);
    dwc_wdt_clr_it(wdt);
    dwc_wdt_reload(wdt);
}

