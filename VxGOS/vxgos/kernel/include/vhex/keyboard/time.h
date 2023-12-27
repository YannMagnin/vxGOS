#ifndef __VHEX_KEYBOARD_TIME__
# define __VHEX_KEYBOARD_TIME__

#include <vhex/keyboard/types.h>

//---
//  Time frequency settings
//
//  Unlike Gint, I use the KeyScan Interface for the key matrix scanning, but I
//  whant to keep the notion of "time" to know the elapsed time between each key
//  strokes, because this information is usefull for game which whant to analyse
//  combo.
//
//  So, I use a ETMU whith no interruption which refert to the "time" indicator,
//  and this part of the API is usefull to change the granularity of said timer.
//---

/* keysc_time_frequency(): Get the current keyboard time frequency in Hertz */
extern int keysc_time_frequency(void);

/* keysc_scan_frequency_us(): Get keyboard time delay in microseconds */
extern uint32_t keysc_time_frequency_us(void);

/* keysc_set_time_frequency(): Set the keyboard scan frequency in Hertz

   The new frequency must be at least 64 for the keyboard to work reliably, and
   at most 32768 for the underlying ETMU to support it. Out-of-range values are
   forced to the closest valid value.

   @freq  New scan frequency, in Hertz */
extern void keysc_set_time_frequency(int freq);



#endif /* __VHEX_KEYBOARD_TIME__ */
