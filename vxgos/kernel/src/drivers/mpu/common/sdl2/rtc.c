#include <vhex/rtc.h>
#include <vhex/rtc/interface.h>
#include <vhex/driver.h>

#include <time.h>
#include <stdio.h>

/* sdl_rtc_get_time(): Read the current time from the RTC */
int sdl_rtc_get_time(rtc_time_t *rtc)
{
    time_t rtime;
    struct tm *tinfo;

    if (rtc == NULL)
        return -1;

    time(&rtime);
    tinfo = localtime(&rtime);

    rtc->ticks     = 0;
    rtc->seconds   = tinfo->tm_sec;
    rtc->minutes   = tinfo->tm_min;
    rtc->hours     = tinfo->tm_hour;
    rtc->month_day = tinfo->tm_mday;
    rtc->month     = tinfo->tm_mon;
    rtc->year      = tinfo->tm_year + 1900;
    rtc->week_day  = tinfo->tm_wday;
    return 0;
}

//---
// Fake driver definition
//---

/* __rtc_configure() : configure the RTC */
static void __rtc_configure(void)
{
    // Nothing to do, this is a fake driver
    printf("[drv] RTC: fake driver initialized\n");
}

/* __rtc_hsave() : save hardware information */
static void __rtc_hsave(void)
{
    // Nothing to do, this is a fake driver
    ;
}

/* __rtc_hrestore() : restore hardware information */
static void __rtc_hrestore(void)
{
    // Nothing to do, this is a fake driver
    ;
}

struct vhex_driver drv_rtc = {
    .name       = "SDL2 RTC",
    .hsave      = (void*)&__rtc_hsave,
    .hrestore   = (void*)&__rtc_hrestore,
    .configure  = (void*)&__rtc_configure,
    .state_size = 4,
    .flags = {
        .RTC      = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct rtc_drv_interface){
        .rtc_get_time = &sdl_rtc_get_time,
        .rtc_set_time = NULL,
    }
};
VHEX_DECLARE_DRIVER(06, drv_rtc);
