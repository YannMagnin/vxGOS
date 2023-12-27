#include <vhex/driver/mpu/sh/sh7305/usb.h>
#include <vhex/display.h>
#include <vhex/usb.h>

/* sh7305_usb_interrupt_handler() : interrupt handler invoked by hardware */
void sh7305_usb_interrupt_handler(void)
{   static char const * const device_st[] = {
        "powered", "default", "address", "configured",
        "suspended-powered", "suspended-default", "suspended-address",
        "suspended-configured",
    };
    /* Save PIPESEL to avoid concurrent access issues */
    uint16_t pipesel = SH7305_USB.PIPESEL.word;

    if(SH7305_USB.INTSTS0.VBINT)
    {
        SH7305_USB_INTSTS0_CLEAR(VBINT);
        usb_logger_write("VBUS %s\n", SH7305_USB.INTSTS0.VBSTS ? "up" : "down");
    }
    else if(SH7305_USB.INTSTS0.CTRT)
    {
        SH7305_USB_INTSTS0_CLEAR(CTRT);
        if(SH7305_USB.INTSTS0.VALID)
            sh7305_usb_request_handle();
    }
    else if(SH7305_USB.INTSTS0.DVST)
    {
        SH7305_USB_INTSTS0_CLEAR(DVST);
        usb_logger_write("DVST %s", device_st[SH7305_USB.INTSTS0.DVSQ]);
        if(SH7305_USB.INTSTS0.DVSQ == SH7305_USB_DVSQ_ADDRESS) {
            usb_logger_write(": %04x\n", SH7305_USB.USBADDR.word);
        } else {
            usb_logger_write("\n");
        }

        //TODO : handle config
    }
    else {
        dclear(C_WHITE);
        dprint(
            0, 0, C_BLACK,
            "USB INTERRUPT HANDLER (%02X) !!!\n",
            SH7305_USB.INTSTS0.word
        );
        dupdate();
        while (1) { __asm__ volatile ("nop"); }
    }

    /* Restore PIPESEL which can have been used for transfers */
    SH7305_USB.PIPESEL.word = pipesel;
}
