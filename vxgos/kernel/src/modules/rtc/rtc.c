#include <vhex/rtc/interface.h>
#include <vhex/rtc.h>
#include <vhex/module.h>
#include <vhex/driver.h>

#include <string.h>

/* internal timer information */
struct {
    struct rtc_drv_interface driver;
} rtc_info;

//---
// user-level timer API
//---

/* rtc_get_time(): Read the current time from the RTC */
int rtc_get_time(rtc_time_t *time)
{
    if (rtc_info.driver.rtc_get_time != NULL)
        return rtc_info.driver.rtc_get_time(time);
    return -1;
}

/* rtc_set_time(): Set current time in the RTC */
int rtc_set_time(rtc_time_t const *time)
{
    if (rtc_info.driver.rtc_set_time != NULL)
        return rtc_info.driver.rtc_set_time(time);
    return -1;
}

//---
// Kernel module information
//---

/* __rtc_init() : initialize the RTC module */
static void __rtc_init(void)
{
    memset(&rtc_info, 0x00, sizeof(rtc_info));

    struct vhex_driver *driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.RTC) {
            memcpy(
                &rtc_info.driver,
                driver[i].module_data,
                sizeof(struct rtc_drv_interface)
            );
            break;
        }
    }
}

/* __rtc_quit() : uninitialize the RTC module */
static void __rtc_quit(void)
{
    ;
}

/* declare the timer module */

struct vhex_module mod_rtc = {
    .name = "RTC",
    .init = &__rtc_init,
    .quit = &__rtc_quit,
};
VHEX_DECLARE_MODULE(06, mod_rtc);
