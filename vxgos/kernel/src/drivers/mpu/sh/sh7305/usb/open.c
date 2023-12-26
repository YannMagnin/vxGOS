#include <vhex/driver/mpu/sh/sh7305/usb.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/usb/configuration.h>
#include <vhex/usb.h>

//---
// Public API
//---

/* sh7305_usb_open() : power on the module and setup USB module */
int sh7305_usb_open(struct usb_full_device *device)
{
    int ret;

    usb_logger_write("-- Open USB ---\n");

    /* check and update device configuration */
    usb_logger_write("Configuration check...\n");
    ret = usb_config_resolve(device);
    if (ret != 0)
        return ret;

    /* handle hardware power routine */
    if (sh7305_usb_hpowered() == false) {
        usb_logger_write("Poweron the hardware module...\n");
        sh7305_usb_hpoweron();
    }

    /* DCP module configuration  */
    usb_logger_write("USB DCP pipe initialization...\n");

    /* Turn on the module now that SCKE=1
     * @notes
     * - USBE  =1 enable USB operation
     * - HSE   =1 enable high-speed operation
     * - DCFM  =0 select the "function" controller mode
     * - DRPD  =0 disable pulling down the line D+/D- (host only)
     * - DPRPU =1 pull D+ up to 3.3V, notifying connection when possible */
    SH7305_USB.SYSCFG.USBE  = 1;
    SH7305_USB.SYSCFG.HSE   = 1;
    SH7305_USB.SYSCFG.DCFM  = 0;
    SH7305_USB.SYSCFG.DRPD  = 0;
    SH7305_USB.SYSCFG.DPRPU = 1;

    /* Read SYSSTS.LNST as required after modifying DPRPU */
    VUNUSED volatile int LNST = SH7305_USB.SYSSTS.LNST;

    /* Prepare the default control pipe.
     * @notes
     * - DIR    =0  data receiving direction
     * - DEVSEL =0  select the device 0b0000 (host selection)
     * - MXPS   =64 select 64 bytes max DCP package size (default size) */
    SH7305_USB.DCPCFG.DIR     = 0;
    SH7305_USB.DCPMAXP.DEVSEL = 0b0000;
    SH7305_USB.DCPMAXP.MXPS   = 64;

    /* Prepare Start of Frame (SOF)
     * @notes
     * - TRNENSELF =0 should be set to 0 for fonction controller mode
     * - BRDYM     =0 force sofware clearing BRDY interrupt flag
     * - HIGH      =1 should be set to 1 */
    SH7305_USB.SOFCFG.TRNENSEL = 0;
    SH7305_USB.SOFCFG.BRDYM    = 0;
    SH7305_USB.SOFCFG.HIGH     = 1;

    /* Pipes configuration */
    usb_logger_write("USB pipes configuration...\n");

    /* Clear D0FIFOSEL and D1FIFOSEL so that pipes can be configured */
    SH7305_USB.D0FIFOSEL.word = 0x0000;
    SH7305_USB.D1FIFOSEL.word = 0x0000;

    usb_logger_write("WARNING : missing pipes configuration...\n");
    //TODO : configure pipes
    //TODO : handle configuration

    /* interrupt setup */
    usb_logger_write("USB interrupt setup...\n");

    /* Configure general USB interruption
     * @notes
     * - VBSE  =1 enable VBUS interruption
     * - RSME  =0 disable Resume interruption
     * - SOFE  =0 disable Frame Number Update interruption
     * - DVSE  =1 enable Device State Transition
     * - CTRE  =1 enable Control Transfert Stage Transition
     * - BEMPE =0 disable Buffer Empty interrupt
     * - NRDYE =0 disable Buffer Not Ready interrupt
     * - BRDYE =0 disable Buffer Ready interrupt */
    SH7305_USB.INTENB0.VBSE = 1;
    SH7305_USB.INTENB0.RSME = 0;
    SH7305_USB.INTENB0.SOFE = 0;
    SH7305_USB.INTENB0.DVSE = 1;
    SH7305_USB.INTENB0.CTRE = 1;
    SH7305_USB.INTENB0.BEMPE = 0;
    SH7305_USB.INTENB0.NRDYE = 0;
    SH7305_USB.INTENB0.BEMPE = 0;

    //TODO : proper interrupt handling
    SH7305_USB.BRDYENB.word = 0x0000;
    SH7305_USB.NRDYENB.word = 0x0000;
    SH7305_USB.BEMPENB.word = 0x0000;

    /* clear interrupt flags */
    SH7305_USB_INTSTS0_CLEAR(VBINT);
    SH7305_USB_INTSTS0_CLEAR(RESM);
    SH7305_USB_INTSTS0_CLEAR(SOFR);
    SH7305_USB_INTSTS0_CLEAR(DVST);
    SH7305_USB_INTSTS0_CLEAR(CTRT);
    SH7305_USB_INTSTS0_CLEAR(VALID);

    /* install hardware interrupt handler and enable USB interruption */
    sh7305_intc_install_inth_generic(
        0xa20,
        VHEX_CALL(&sh7305_usb_interrupt_handler)
    );
    sh7305_intc_priority(INTC_USB, 8);

    usb_logger_write("USB now waiting interruption\n");
    return 0;
}
