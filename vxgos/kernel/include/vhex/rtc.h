#ifndef __VHEX_DRIVER_RTC__
# define __VHEX_DRIVER_RTC__

#include <vhex/rtc/types.h>

/* rtc_get_time(): Read the current time from the RTC
   @time  Pointer to rtc_time_t structure (needs not be initialized) */
extern int rtc_get_time(rtc_time_t *time);

/* rtc_set_time(): Set current time in the RTC
   If [time->week_day] is not in the valid range, it is set to 0. Other fields
   are not checked. R64CNT cannot be set to [time->ticks] is ignored.
   @time  Pointer to new time */
extern int rtc_set_time(rtc_time_t const *time);

#endif /* __VHEX_DRIVER_RTC__ */
