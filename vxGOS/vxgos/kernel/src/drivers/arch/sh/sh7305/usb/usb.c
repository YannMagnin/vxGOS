#include <vhex/driver/mpu/sh/sh7305/usb.h>
#include <vhex/driver/mpu/sh/sh7305/cpg.h>
#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>
#include <vhex/usb/interface.h>
#include <vhex/driver.h>

//----
// Public kernel-level driver declaration
//----

/* sh7305_usb_hpowered() : check if the USB module is powered */
bool sh7305_usb_hpowered(void)
{
    return (SH7305_CPG.USBCLKCR.CLKSTP == 0) && (SH7305_POWER.MSTPCR2.USB == 0);
}

/* sh7305_usb_hpoweron() : power on the USB module */
int sh7305_usb_hpoweron(void)
{
    if(sh7305_usb_hpowered())
        return 0;

    /* TODO: USB: Proper handling of MSELCRA and MSELCRB */
    uint16_t volatile *MSELCRA = (void *)0xa4050180;
    uint16_t volatile *MSELCRB = (void *)0xa4050182;
    *MSELCRA &= 0xff3f;
    *MSELCRB &= 0x3fff;

    /* Leave some delay for the clock to settle. The OS leaves
       100 ms, but it just never seems necessary. */
    SH7305_CPG.USBCLKCR.CLKSTP = 0;
    sh7305_tmu_sleep_us_spinwait(1000);

    /* enable USB power and clock supplie.
     * @notes
     * - we manually force UPONCR because Casio seems to badly restore this
     *   register by writting undocumented bits */
    SH7305_POWER.MSTPCR2.USB = 0;
    SH7305_USB_UPONCR.word = 0x0600;

    /* Turn on SCKE, which activates all other registers. The existing
     * BUSWAIT delay might not be high enough, so wait a little bit before
     * modifying registers; a couple CPU cycles is enough. */
    SH7305_USB.SYSCFG.SCKE = 1;
    for(int i = 0; i < 10; i++) __asm__ volatile("nop");

    /* Set BUSWAIT to a safe value */
    SH7305_USB.BUSWAIT.word = 5;

    /* hardcode this deeply undocumented register to Casio value */
    *(uint16_t volatile *)0xa4d800c2 = 0x0020;
    return 0;
}

/* sh7305_usb_hpoweroff() : power off the USB module */
int sh7305_usb_hpoweroff(void)
{
    uint16_t volatile *MSELCRA = (void *)0xa4050180;
    uint16_t volatile *MSELCRB = (void *)0xa4050182;

    /* Power off the USB (procedure at section 31.5.1 of the SH7724)
     * This delay is crucial and omitting it has caused constant freezes in
     * the past. Blame undocumented clock magic? */
    SH7305_USB_UPONCR.word = 0x0000;
    sh7305_tmu_sleep_us_spinwait(1000);

    /* Stop clock supplying */
    SH7305_POWER.MSTPCR2.USB = 1;
    SH7305_CPG.USBCLKCR.CLKSTP = 1;
    sh7305_tmu_sleep_us_spinwait(1000);

    /* The values used by the OS (a PFC driver could do better) */
    *MSELCRB = (*MSELCRB & 0x3fff) | 0xc000;
    *MSELCRA = (*MSELCRA & 0xff3f) | 0x0040;
    return 0;
}

//---
// Internal kernel-lever driver definition
//---

/* declare driver */
struct vhex_driver drv_usb = {
    .name       = "USB",
    .hpowered   = (void*)&sh7305_usb_hpowered,
    .hpoweron   = (void*)&sh7305_usb_hpoweron,
    .hpoweroff  = (void*)&sh7305_usb_hpoweroff,
    .hsave      = NULL,
    .hrestore   = NULL,
    .configure  = NULL,
    .state_size = 4,
    .flags = {
        .USB    = 1,
        .SHARED = 0,
        .UNUSED = 1,
    },
    .module_data = &(struct usb_drv_interface){
        .open      = &sh7305_usb_open,
        .pipe_find = &sh7305_usb_pipe_find
    }
};
VHEX_DECLARE_DRIVER(06, drv_usb);
