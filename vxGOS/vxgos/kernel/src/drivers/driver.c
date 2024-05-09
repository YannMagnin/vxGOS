//---
// drivers:driver   - driver abstraction
//---

#include <string.h>

#include "vhex/drivers/driver.h"
#include "vhex/_klibc.h"

//---
// Public
//---

/* disable out-of-bounds analysing since we use linker-script magic */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-out-of-bounds"
#pragma GCC diagnostic ignored "-Wanalyzer-allocation-size"

/* kernel_driver_next() : feth the next driver */
int kernel_driver_next(struct vxdriver **driver)
{
    extern uintptr_t __vhex_drivers_array_start;
    extern uintptr_t __vhex_drivers_array_end;

    if (driver == NULL)
        return -1;
    if (driver[0] == NULL) {
        driver[0] = (struct vxdriver*)&__vhex_drivers_array_start;
    } else {
        driver[0] = &(driver[0][1]);
        if ((uintptr_t)driver[0] >= (uintptr_t)&__vhex_drivers_array_end) {
            driver[0] = NULL;
            return 1;
        }
    }
    return 0;
}

/* kernel_driver_find() : find a particular driver */
int kernel_driver_find(struct vxdriver **driver, char *name)
{
    int special_display;
    int special_keyboard;
    int special_rtc;
    int special_timer;
    int special_dma;
    int special_usb;

    if (driver == NULL)
        return -1;
    special_display     = !klibc_strcmp("display", name);
    special_keyboard    = !klibc_strcmp("keyboard", name);
    special_rtc         = !klibc_strcmp("rtc", name);
    special_timer       = !klibc_strcmp("timer", name);
    special_dma         = !klibc_strcmp("dma", name);
    special_usb         = !klibc_strcmp("usb", name);
    driver[0] = NULL;
    while (true)
    {
        if (kernel_driver_next(driver) != 0)
            break;
        if (special_display && (driver[0])->flags.DISPLAY)
            return 0;
        if (special_keyboard && (driver[0])->flags.KEYBOARD)
            return 0;
        if (special_rtc && (driver[0])->flags.RTC)
            return 0;
        if (special_timer && (driver[0])->flags.TIMER)
            return 0;
        if (special_dma && (driver[0])->flags.DMA)
            return 0;
        if (special_usb && (driver[0])->flags.USB)
            return 0;
        if (klibc_strcmp(name, (driver[0])->name) == 0)
            return 0;
    }
    driver[0] = NULL;
    return -2;
}

/* restore GCC context (re-enable out-of-bounds checks */
#pragma GCC diagnostic pop
