#ifndef __VHEX_TIMER_INTERFACE__
# define __VHEX_TIMER_INTERFACE__

#include <vhex/rtc/types.h>

/* rtc_drv_interface - driver interface */
struct rtc_drv_interface
{
    int (*rtc_get_time)(rtc_time_t *time);
    int (*rtc_set_time)(rtc_time_t const *time);
};

#endif /* __VHEX_TIMER_INTERFACE__ */
