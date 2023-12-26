#ifndef __VHEX_DRIVER_MPU_SH_SH7305_RTC__
# define __VHEX_DRIVER_MPU_SH_SH7305_RTC__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>
#include <vhex/defs/call.h>

//---
//  Hybrid SH7705-SH7305 Real-Time Clock. Refer to:
//    "Renesas SH7705 Group Hardware Manual"
//    Section 15: "Real-Time Clock (RTC)"
//    "Renesas SH7724 User's Manual: Hardware"
//    Section 28: "Real-Time Clock (RTC)"
//---

/* rtc_BCD2_t - a 2-digit BCD counter with a 1-byte gap */
typedef struct
{
    byte_union(,
        uint8_t TENS        :4;
        uint8_t ONES        :4;
    );
    pad(1);

} VPACKED(2) rtc_BCD2_t;

/* sh7305_rtc - Date and time access, RTC control */
struct __sh7305_rtc_s
{
    uint8_t const R64CNT;       /* A 64-Hz counter */
    pad(1);

    rtc_BCD2_t RSECCNT;         /* Second count */
    rtc_BCD2_t RMINCNT;         /* Minute count */
    rtc_BCD2_t RHRCNT;          /* Hour count */

    uint8_t RWKCNT;             /* Day of week, must be in [0..6] */
    pad(1);

    rtc_BCD2_t RDAYCNT;         /* Day count */
    rtc_BCD2_t RMONCNT;         /* Month count */

    word_union(RYRCNT,          /* Year count */
        uint THOUSANDS      :4;
        uint HUNDREDS       :4;
        uint TENS           :4;
        uint ONES           :4;
    );
    pad(12);                    /* Alarm registers... */

    byte_union(RCR1,
        uint8_t CF          :1; /* Carry flag */
        uint8_t             :2;
        uint8_t CIE         :1; /* Carry interrupt enable */
        uint8_t AIE         :1; /* Alarm interrupt enable */
        uint8_t             :2;
        uint8_t AF          :1; /* Alarm flag */
    );
    pad(1);

    byte_union(RCR2,
        uint8_t PEF         :1; /* Periodic interrupt flag */
        uint8_t PES         :3; /* Periodic interrupt interval */
        uint8_t             :1;
        uint8_t ADJ         :1; /* 30-second adjustment */
        uint8_t RESET       :1; /* Reset trigger */
        uint8_t START       :1; /* Start bit */
    );
    pad(1);

    byte_union(RCR3,
        uint8_t BEN         :1; /* Periodic interrupt flag */
        uint8_t             :7;
    );
    pad(1);
} VPACKED(4);


#define SH7305_RTC (*((volatile struct __sh7305_rtc_s *)0xa413fec0))

//---
//  RTC periodic interrupt
//  The real-time clock produces a regular interrupt which may be used as a
//  timer with a maximum frequency of 256 Hz. It is also useful to check
//  that the clock settings (see <gint/clock.h>) are properly detected, by
//  comparing the detected frequencies with the RTC.
//---

/* Possible frequency settings for the RTC's interrupt */
enum
{
    RTC_500mHz  = 7,
    RTC_1Hz     = 6,
    RTC_2Hz     = 5,
    RTC_4Hz     = 4,
    RTC_16Hz    = 3,
    RTC_64Hz    = 2,
    RTC_256Hz   = 1,
    RTC_NONE    = 0,
};

/* rtc_periodic_enable(): Enable the periodic interrupt

   This function sets up the periodic interrupt to invoke the provided callback
   regularly. As with timers, the callback must return either TIMER_CONTINUE or
   TIMER_STOP.

   Note that the timing of the first callback is always uncertain. A 1 Hz timer
   set up when half of the current second is already elapsed will be called for
   the first time after only 500 ms, for instance.

   @frequency  Periodic interrupt frequency
   @callback   Function to call back at the specified frequency
   Returns true on success, false if the interrupt is already in use. */
extern bool sh7305_rtc_periodic_enable(int frequency, vhex_call_t callback);

/* rtc_periodic_disable(): Stop the periodic interrupt

   This has the same effect as returning TIMER_STOP from the callback, or
   setting RTC_NONE as the parameter for rtc_periodic_enable(). */
extern void sh7305_rtc_periodic_disable(void);

#endif /* __VHEX_DRIVER_MPU_SH_SH7305_RTC__ */
