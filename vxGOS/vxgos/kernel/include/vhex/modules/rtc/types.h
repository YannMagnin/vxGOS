#ifndef __VHEX_RTC_TYPES__
# define __VHEX_RTC_TYPES__

#include <stdint.h>

/* rtc_time_t: A point in time, representable in the RTC registers

   WARNING: A copy of this definition is used in fxlibc, make sure to keep it
   in sync. (We don't install kernel headers before compiling the fxlibc, it's
   kind of a nightmare for the modest build system.) */
typedef struct
{
    uint16_t year;      /* Years (exact value, e.g. 2018) */
    uint8_t week_day;   /* Day of week, (0=Sunday, 6=Saturday) */
    uint8_t month;      /* Month (0..11) */
    uint8_t month_day;  /* Day of month (1..31) */
    uint8_t hours;      /* Hour (0..23) */
    uint8_t minutes;    /* Minute (0..59) */
    uint8_t seconds;    /* Second (0..59) */
    uint8_t ticks;      /* 128-Hz sub-second counter (0...127) */

} rtc_time_t;

#endif /* __VHEX_RTC_TYPES__ */
