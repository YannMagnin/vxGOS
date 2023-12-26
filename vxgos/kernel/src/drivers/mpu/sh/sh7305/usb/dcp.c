#include <vhex/driver/mpu/sh/sh7305/usb.h>

/* sh7305_usb_dcp_write() : send data through DCP pipe */
int sh7305_usb_dcp_write(void *buffer, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        *((volatile uint8_t*)&SH7305_USB.CFIFO) = ((uint8_t*)buffer)[i];
    }
    return n;
}

/* sh7305_usb_dcp_prepare() : prepare FIFO buffer */
void sh7305_usb_dcp_prepare(void)
{
    /* Configure Control FIFO pipe.
     * @config
     * - RCNT    =1       the CFIFOCTR.DTLN is decremented only during reading
     * - REW     =0       buffer pointer is not rewound
     * - MBW     =0b00    select 8bits access
     * - BIGEND  =1       use big endian
     * - ISEL    =1       select writting pipe
     * - CURPIPE =0b0000  select DCP pipe
     * @notes
     * - as the SH7724 documentation explain at section 31.4.8, we cannot
     *   change the bits access during reading or writing once setuped. So, we
     *   use 8-bits writing to avoid buffer alignement fault during
     *   configuration transaction. */
    SH7305_USB.CFIFOSEL.RCNT    = 1;
    SH7305_USB.CFIFOSEL.REW     = 0;
    SH7305_USB.CFIFOSEL.MBW     = 0b00;
    SH7305_USB.CFIFOSEL.BIGEND  = 1;
    SH7305_USB.CFIFOSEL.CURPIPE = 0b0000;
    SH7305_USB_REG_SYNC(SH7305_USB.CFIFOSEL.CURPIPE != 0b0000);
    SH7305_USB.CFIFOSEL.ISEL    = 1;
    SH7305_USB_REG_SYNC(SH7305_USB.CFIFOSEL.ISEL == 0);

    /* wait until the CDP buffer is available */
    SH7305_USB_REG_SYNC(SH7305_USB.DCPCTR.BSTS == 0);

    /* send data */
    SH7305_USB.DCPCTR.PID = 1;
}

/* sh7305_usb_dcp_commit() : send data */
void sh7305_usb_dcp_commit(void)
{
    /* wait buffer sync. and indicate that the writing has beens finished /
     * @notes
     * - SH7724 section 31.4.8 */
    SH7305_USB_REG_SYNC(SH7305_USB.CFIFOCTR.FRDY == 0);
    SH7305_USB.CFIFOCTR.BVAL = 1;

    /* finish the control transfert stage
     * @notes
     * - SH7724 section 31.4.29 */
    SH7305_USB.DCPCTR.CCPL   = 1;
}
