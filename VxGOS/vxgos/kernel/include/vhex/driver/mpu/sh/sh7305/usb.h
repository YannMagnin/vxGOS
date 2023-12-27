#ifndef __VHEX_MPU_SH7305_USB__
# define __VHEX_MPU_SH7305_USB__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

struct __sh7305_usb
{
    /* SYSCFG -  System Configuration Control Register */
    word_union(SYSCFG,
        uint16_t const  : 5;
        uint16_t SCKE   : 1;    /* USB clock module controller */
        uint16_t const  : 1;
        uint16_t const  : 1;
        uint16_t HSE    : 1;    /* High-speed operations enable */
        uint16_t DCFM   : 1;    /* Function controller */
        uint16_t DRPD   : 1;    /* D+/- Resistor Control */
        uint16_t DPRPU  : 1;    /* D+ Resistor Control */
        uint16_t const  : 3;
        uint16_t USBE   : 1;    /* USB operation enable */
    );

    /* BUSWAIT - CPU Bus Wait Setting Register */
    word_union(BUSWAIT,
        uint16_t const  :12;
        uint16_t BWAIT  : 4;    /* CPU Bus wait */
    );

    /* SYSSTS - System Configuration Status Register
     * @notes
     * - emulator allows writting LNST */
    word_union(SYSSTS,
        uint16_t const  : 5;
        uint16_t HIGH   : 1;    /* Should be 1 */
        uint16_t const  : 8;
        uint16_t LNST   : 2;    /* USB Data Line Status */
    );
    pad(0x02);

    /* DVTSTCCTR - Device State Controle Register */
    word_union(DVSTCTR,
        uint16_t const  : 7;
        uint16_t WKUP   : 1;    /* Wakeup Output */
        uint16_t RWUPE  : 1;    /* Wakeup Detection Enable */
        uint16_t USBRT  : 1;    /* USB Reset Output */
        uint16_t RESUME : 1;    /* Resume Output */
        uint16_t UACT   : 1;    /* USB Bus Enable */
        uint16_t const  : 1;
        uint16_t RHST   : 3;    /* Reset Handshake */
    );
    pad(0x02);

    /* TESTMODE - Test Mode Register */
    word_union(TESTMODE,
        uint16_t const  :12;
        uint16_t UTST   : 4;    /* Test Mode */
    );
    pad(0x06);

    /* [C/Dx]FIFO - FIFO Port Registers */
    uint32_t CFIFO;             /* DCP FIFO port */
    uint32_t D0FIFO;            /* Data 0 FIFO port */
    uint32_t D1FIFO;            /* Data 1 FIFO port */

    /* CFIFOSEL - FIFO Port Select Register
     * @notes
     * - emulator register mask is     0xcd4f
     * - physical (and SH7724) mask is 0xcd2f (ISEL shifted) */
    word_union(CFIFOSEL,
        uint16_t RCNT   : 1;    /* Read Count Mode */
        uint16_t REW    : 1;    /* Pointer Buffer Rewind */
        uint16_t const  : 2;
        uint16_t MBW    : 2;    /* Access Bits Width */
        uint16_t const  : 1;
        uint16_t BIGEND : 1;    /* Endiant Control */
        uint16_t const  : 2;
        uint16_t ISEL   : 1;    /* Access Direction When DCP is used */
        uint16_t const  : 1;
        uint16_t CURPIPE :4;    /* Port Access Pipe Specification */
    );
    /* CFIFOCTR - FIFO Port Control Register */
    word_union(CFIFOCTR,
        uint16_t BVAL   : 1;    /* Buffer Memory Valid Flags */
        uint16_t BCLR   : 1;    /* CPU Buffer Clear */
        uint16_t FRDY   : 1;    /* FIFO Port Ready */
        uint16_t const  : 1;
        uint16_t DTLN   :12;    /* Receive Data Length */
    );
    pad(0x04);

    /* D0FIFOSEL - FIFO Port Select Register */
    word_union(D0FIFOSEL,
        uint16_t RCNT   : 1;    /* Read Count Mode */
        uint16_t REW    : 1;    /* Pointer Buffer Rewind */
        uint16_t DCLRM  : 1;    /* Auto Buffer Clear Mode */
        uint16_t DREQE  : 1;    /* DMA Transfert Request Enable */
        uint16_t MBW    : 2;    /* Access Bits Width */
        uint16_t const  : 1;
        uint16_t BIGEND : 1;    /* Endiant Control */
        uint16_t const  : 4;
        uint16_t CURPIPE :4;    /* Port Access Pipe Specification */
    );
    /* D0FIFOCTR - FIFO Port Control Register */
    word_union(D0FIFOCTR,
        uint16_t BVAL   : 1;    /* Buffer Memory Valid Flags */
        uint16_t BCLR   : 1;    /* CPU Buffer Clear */
        uint16_t FRDY   : 1;    /* FIFO Port Ready */
        uint16_t const  : 1;
        uint16_t DTLN   :12;    /* Receive Data Length */
    );

    /* D1FIFOSEL - FIFO Port Select Register */
    word_union(D1FIFOSEL,
        uint16_t RCNT   : 1;    /* Read Count Mode */
        uint16_t REW    : 1;    /* Pointer Buffer Rewind */
        uint16_t DCLRM  : 1;    /* Auto Buffer Clear Mode */
        uint16_t DREQE  : 1;    /* DMA Transfert Request Enable */
        uint16_t MBW    : 2;    /* Access Bits Width */
        uint16_t const  : 1;
        uint16_t BIGEND : 1;    /* Endiant Control */
        uint16_t const  : 4;
        uint16_t CURPIPE :4;    /* Port Access Pipe Specification */
    );
    /* D1FIFOCTR - FIFO Port Control Register */
    word_union(D1FIFOCTR,
        uint16_t BVAL   : 1;    /* Buffer Memory Valid Flags */
        uint16_t BCLR   : 1;    /* CPU Buffer Clear */
        uint16_t FRDY   : 1;    /* FIFO Port Ready */
        uint16_t const  : 1;
        uint16_t DTLN   :12;    /* Receive Data Length */
    );

    /* INTENB0 - Interrupt Enable Register */
    word_union(INTENB0,
        uint16_t VBSE   : 1;    /* VBUS */
        uint16_t RSME   : 1;    /* Resume */
        uint16_t SOFE   : 1;    /* Frame Number Update */
        uint16_t DVSE   : 1;    /* Device State Transition */
        uint16_t CTRE   : 1;    /* Control Transfert Stage Transition */
        uint16_t BEMPE  : 1;    /* Buffer Empty */
        uint16_t NRDYE  : 1;    /* Buffer Not Ready */
        uint16_t BRDYE  : 1;    /* Buffer Ready */
        uint16_t const  : 8;
    );
    pad(4);

    /* BRDYENB - BRDY Interrupt Enable Register */
    word_union(BRDYENB,
        uint16_t const      :6;
        uint16_t PIPE9BRDYE :1; /* BRDY Interrupt enable for PIPE9 */
        uint16_t PIPE8BRDYE :1; /* BRDY Interrupt enable for PIPE8 */
        uint16_t PIPE7BRDYE :1; /* BRDY Interrupt enable for PIPE7 */
        uint16_t PIPE6BRDYE :1; /* BRDY Interrupt enable for PIPE6 */
        uint16_t PIPE5BRDYE :1; /* BRDY Interrupt enable for PIPE5 */
        uint16_t PIPE4BRDYE :1; /* BRDY Interrupt enable for PIPE4 */
        uint16_t PIPE3BRDYE :1; /* BRDY Interrupt enable for PIPE3 */
        uint16_t PIPE2BRDYE :1; /* BRDY Interrupt enable for PIPE2 */
        uint16_t PIPE1BRDYE :1; /* BRDY Interrupt enable for PIPE1 */
        uint16_t PIPE0BRDYE :1; /* BRDY Interrupt enable for PIPE0 */
    );
    /* NRDYENB - NRDY Interrupt Enable Register */
    word_union(NRDYENB,
        uint16_t const      :6;
        uint16_t PIPE9NRDYE :1; /* NRDY Interrupt enable for PIPE9 */
        uint16_t PIPE8NRDYE :1; /* NRDY Interrupt enable for PIPE8 */
        uint16_t PIPE7NRDYE :1; /* NRDY Interrupt enable for PIPE7 */
        uint16_t PIPE6NRDYE :1; /* NRDY Interrupt enable for PIPE6 */
        uint16_t PIPE5NRDYE :1; /* NRDY Interrupt enable for PIPE5 */
        uint16_t PIPE4NRDYE :1; /* NRDY Interrupt enable for PIPE4 */
        uint16_t PIPE3NRDYE :1; /* NRDY Interrupt enable for PIPE3 */
        uint16_t PIPE2NRDYE :1; /* NRDY Interrupt enable for PIPE2 */
        uint16_t PIPE1NRDYE :1; /* NRDY Interrupt enable for PIPE1 */
        uint16_t PIPE0NRDYE :1; /* NRDY Interrupt enable for PIPE0 */
    );
    /* BEMPENB - BEMP Interrupt Enable Register */
    word_union(BEMPENB,
        uint16_t const      :6;
        uint16_t PIPE9BEMPE :1; /* BEMP Interrupt enable for PIPE9 */
        uint16_t PIPE8BEMPE :1; /* BEMP Interrupt enable for PIPE8 */
        uint16_t PIPE7BEMPE :1; /* BEMP Interrupt enable for PIPE7 */
        uint16_t PIPE6BEMPE :1; /* BEMP Interrupt enable for PIPE6 */
        uint16_t PIPE5BEMPE :1; /* BEMP Interrupt enable for PIPE5 */
        uint16_t PIPE4BEMPE :1; /* BEMP Interrupt enable for PIPE4 */
        uint16_t PIPE3BEMPE :1; /* BEMP Interrupt enable for PIPE3 */
        uint16_t PIPE2BEMPE :1; /* BEMP Interrupt enable for PIPE2 */
        uint16_t PIPE1BEMPE :1; /* BEMP Interrupt enable for PIPE1 */
        uint16_t PIPE0BEMPE :1; /* BEMP Interrupt enable for PIPE0 */
    );

    /* SOFCFG - SOF Control Register */
    word_union(SOFCFG,
        uint16_t            :7;
        uint16_t TRNENSEL   :1; /* Transaction-Enabled Time Select */
        uint16_t            :1;
        uint16_t BRDYM      :1; /* BRDY Status Clear Timing */
        uint16_t HIGH       :1; /* SHOULD BE SET TO 1 MANUALLY */
        uint16_t            :5;
    );
    pad(2);

    /* Interrupt Status Registers */
    word_union(INTSTS0,
        uint16_t VBINT  : 1; /* VBUS */
        uint16_t RESM   : 1; /* Resume */
        uint16_t SOFR   : 1; /* Frame Number Refresh */
        uint16_t DVST   : 1; /* Device State Transition */
        uint16_t CTRT   : 1; /* Control Transfer Stage Transition */
        uint16_t BEMP   : 1; /* Buffer Empty */
        uint16_t NRDY   : 1; /* Buffer Not Ready */
        uint16_t BRDY   : 1; /* Buffer Ready */
        uint16_t VBSTS  : 1; /* VBUS Input Status */
        uint16_t DVSQ   : 3; /* Device state */
        uint16_t VALID  : 1; /* USB Request Reception */
        uint16_t CTSQ   : 3; /* Control Transfer Stage */
    );
    pad(4);

    /* BRDYENB - BRDY Interrupt Status Register */
    word_union(BRDYSTS,
        uint16_t const      :6;
        uint16_t PIPE9BRDY  :1; /* BRDY Interrupt Status for PIPE9 */
        uint16_t PIPE8BRDY  :1; /* BRDY Interrupt Status for PIPE8 */
        uint16_t PIPE7BRDY  :1; /* BRDY Interrupt Status for PIPE7 */
        uint16_t PIPE6BRDY  :1; /* BRDY Interrupt Status for PIPE6 */
        uint16_t PIPE5BRDY  :1; /* BRDY Interrupt Status for PIPE5 */
        uint16_t PIPE4BRDY  :1; /* BRDY Interrupt Status for PIPE4 */
        uint16_t PIPE3BRDY  :1; /* BRDY Interrupt Status for PIPE3 */
        uint16_t PIPE2BRDY  :1; /* BRDY Interrupt Status for PIPE2 */
        uint16_t PIPE1BRDY  :1; /* BRDY Interrupt Status for PIPE1 */
        uint16_t PIPE0BRDY  :1; /* BRDY Interrupt Status for PIPE0 */
    );
    /* NRDYENB - NRDY Interrupt Status Register */
    word_union(NRDYSTS,
        uint16_t const      :6;
        uint16_t PIPE9NRDY  :1; /* NRDY Interrupt Status for PIPE9 */
        uint16_t PIPE8NRDY  :1; /* NRDY Interrupt Status for PIPE8 */
        uint16_t PIPE7NRDY  :1; /* NRDY Interrupt Status for PIPE7 */
        uint16_t PIPE6NRDY  :1; /* NRDY Interrupt Status for PIPE6 */
        uint16_t PIPE5NRDY  :1; /* NRDY Interrupt Status for PIPE5 */
        uint16_t PIPE4NRDY  :1; /* NRDY Interrupt Status for PIPE4 */
        uint16_t PIPE3NRDY  :1; /* NRDY Interrupt Status for PIPE3 */
        uint16_t PIPE2NRDY  :1; /* NRDY Interrupt Status for PIPE2 */
        uint16_t PIPE1NRDY  :1; /* NRDY Interrupt Status for PIPE1 */
        uint16_t PIPE0NRDY  :1; /* NRDY Interrupt Status for PIPE0 */
    );
    /* BEMPENB - BEMP Interrupt Status Register */
    word_union(BEMPSTS,
        uint16_t const      :6;
        uint16_t PIPE9BEMP  :1; /* BEMP Interrupt Status for PIPE9 */
        uint16_t PIPE8BEMP  :1; /* BEMP Interrupt Status for PIPE8 */
        uint16_t PIPE7BEMP  :1; /* BEMP Interrupt Status for PIPE7 */
        uint16_t PIPE6BEMP  :1; /* BEMP Interrupt Status for PIPE6 */
        uint16_t PIPE5BEMP  :1; /* BEMP Interrupt Status for PIPE5 */
        uint16_t PIPE4BEMP  :1; /* BEMP Interrupt Status for PIPE4 */
        uint16_t PIPE3BEMP  :1; /* BEMP Interrupt Status for PIPE3 */
        uint16_t PIPE2BEMP  :1; /* BEMP Interrupt Status for PIPE2 */
        uint16_t PIPE1BEMP  :1; /* BEMP Interrupt Status for PIPE1 */
        uint16_t PIPE0BEMP  :1; /* BEMP Interrupt Status for PIPE0 */
    );

    /* FRMNUM - Frame Number Register */
    word_union(FRMNUM,
        uint16_t OVRN   : 1; /* Overrun/Underrun Detection Status */
        uint16_t CRCE   : 1; /* Receive Data Error */
        uint16_t        : 3;
        uint16_t FRNM   :11; /* Frame Number */
    );
    /* UFRMNUM - uFrame Number Register */
    word_union(UFRMNUM,
        uint16_t        :13;
        uint16_t UFRNM  : 3; /* uFrame */
    );

    /* USBADDR - USB Address Register
     * @notes
     * - emulator allow writting to this register but is probably only for
     *   register initialization */
    word_union(USBADDR,
        uint16_t            : 9;
        uint16_t USBADDR    : 7; /* USB Address */
    );
    pad(2);

    /* USBREQ - USB Request Type Register */
    word_union(USBREQ,
        uint16_t BREQUEST   :8; /* USB request data value */
        uint16_t BMREQUEST  :8; /* USB request type value */
    );
    /* USBVAL - USB Request Value Register */
    word_union(USBVAL,
        uint16_t WVALUE     :16; /* USB request wValue value */
    );
    /* USBINDX - USB Request Index Register */
    word_union(USBINDX,
        uint16_t WINDEX     :16; /* USB USB request wIndex value */
    );
    /* USBLENG - USB Request Length Register */
    word_union(USBLENG,
        uint16_t WLENGTH    :16; /* USB USB request wLength value */
    );


    /* DCPCFG - DCP Configuration Register */
    word_union(DCPCFG,
        uint16_t            :11;
        uint16_t DIR        : 1; /* Transfer Direction */
        uint16_t            : 4;
    );
    /* DCPMAXP - DCP Maximum Packet Size Register */
    word_union(DCPMAXP,
        uint16_t DEVSEL     : 4; /* Device Select */
        uint16_t            : 5;
        uint16_t MXPS       : 7; /* Maximum Packet Size */
    );
    /* DCPCTR - DCP Control Register */
    word_union(DCPCTR,
        uint16_t BSTS       : 1; /* Buffer Status */
        uint16_t SUREQ      : 1; /* SETUP Token Transmission */
        uint16_t CSCLR      : 1; /* C-SPLIT Status Clear */
        uint16_t CSSTS      : 1; /* C-SPLIT Status */
        uint16_t SUREQCLR   : 1; /* SUREQ Bit Clear */
        uint16_t            : 2;
        uint16_t SQCLR      : 1; /* Toggle Bit Clear */
        uint16_t SQSET      : 1; /* Toggle Bit Set */
        uint16_t SQMON      : 1; /* Sequence Toggle Bit Monitor */
        uint16_t PBUSY      : 1; /* Pipe Busy */
        uint16_t PINGE      : 1; /* PING Token Issue Enable */
        uint16_t            : 1;
        uint16_t CCPL       : 1; /* Control Transfer End Enable */
        uint16_t PID        : 2; /* Response PID */
    );
    pad(2);

    /* PIPESEL - Pipe Window Select Register */
    word_union(PIPESEL,
        uint16_t            :12;
        uint16_t PIPESEL    : 4; /* Pipe Window Select */
    );
    pad(2);
    /* PIPECFG - Pipe Configuration Register */
    word_union(PIPECFG,
        uint16_t TYPE       : 2; /* Transfer Type */
        uint16_t            : 3;
        uint16_t BFRE       : 1; /* BRDY Interrupt Operation Specification */
        uint16_t DBLB       : 1; /* Double Buffer Mode */
        uint16_t CNTMD      : 1; /* Continuous Transfer Mode */
        uint16_t SHTNAK     : 1; /* Pipe Disabled at End of Transfer */
        uint16_t            : 2;
        uint16_t DIR        : 1; /* Transfer Direction */
        uint16_t EPNUM      : 4; /* Endpoint Number */
    );
    /* PIPEBUF - Pipe Buffer Setting Register */
    word_union(PIPEBUF,
        uint16_t            : 1;
        uint16_t BUFSIZE    : 5; /* Buffer Size */
        uint16_t            : 2;
        uint16_t BUFNMB     : 8; /* Buffer Number */
    );
    /* PIPEMAXP - Pipe Maximum Packet Size Register */
    word_union(PIPEMAXP,
        uint16_t DEVSEL     : 4; /* Device Select */
        uint16_t            : 1;
        uint16_t MXPS       :11; /* Maximum Packet Size */
    );

    /* PIPEPERI - Pipe Timing Control Register */
    word_union(PIPEPERI,
        uint16_t            : 3;
        uint16_t IFIS       : 1; /* Isochronous IN Buffer Flush */
        uint16_t            : 9;
        uint16_t IITV       : 3; /* Interval Error Detection Interval */
    );

    /* PIPECTR - PIPEn Control Registers
     * @notes
     * - PIPECTR1 mask = 0xf7e3
     * - PIPECTR2 mask = 0xf7e3
     * - PIPECTR3 mask = 0xf7e3
     * - PIPECTR4 mask = 0xf7e3
     * - PIPECTR5 mask = 0xf7e3
     * - PIPECTR6 mask = 0xb3e3
     * - PIPECTR7 mask = 0xb3e3
     * - PIPECTR8 mask = 0xb3e3
     * - PIPECTR9 mask = 0xb3e3 */
    word_union(PIPECTR[9],
        uint16_t BSTS       : 1; /* Buffer Status */
        uint16_t INBUFM     : 1; /* IN Buffer Monitor (PIPE1..5) */
        uint16_t CSCLR      : 1; /* C-SPLIT Status Clear Bit */
        uint16_t CSSTS      : 1; /* CSSTS Status Bit */
        uint16_t            : 1;
        uint16_t ATREPM     : 1; /* Auto Response Mode (PIPE1..5) */
        uint16_t ACLRM      : 1; /* Auto Buffer Clear Mode */
        uint16_t SQCLR      : 1; /* Toggle Bit Clear */
        uint16_t SQSET      : 1; /* Toggle Bit Set */
        uint16_t SQMON      : 1; /* Toggle Bit Confirmation */
        uint16_t PBUSY      : 1; /* Pipe Busy */
        uint16_t            : 3;
        uint16_t PID        : 2; /* Response PID */
    );
    pad(14);

    /* PIPEnTRE - PIPEn Transaction Counter Enable Registers and */
    /* PIPEnTRN - PIPEn Transaction Counter Registers */
    struct {
        word_union(PIPEnTRE,
            uint16_t        : 6;
            uint16_t TRENB  : 1; /* Transaction Counter Enable */
            uint16_t TRCLR  : 1; /* Transaction Counter Clear */
            uint16_t        : 8;
        );
        word_union(PIPEnTRN,
            uint16_t TRCNT  :16; /* Transaction Counter */
        );
    } PIPETR[5];
};

/* UPONCR - USB Power Control Register */
union __sh7305_usb_uponcr
{
    uint16_t word;
    struct {
        uint16_t const          : 5;
        uint16_t UPON           : 2; /* USB Power Control */
        uint16_t const          : 9;
    } __attribute__((packed, aligned(2)));
} __attribute__((packed, aligned(2)));


#define SH7305_USB        (*(volatile struct __sh7305_usb       *)0xa4d80000)
#define SH7305_USB_UPONCR (*(volatile union __sh7305_usb_uponcr *)0xa40501d4)

//---
// Kernel-level USB API
//---

/* SH7305_USB_DVSQ_* - device state value */
enum {
    SH7305_USB_DVSQ_POWERED             = 0,
    SH7305_USB_DVSQ_DEFAULT             = 1,
    SH7305_USB_DVSQ_ADDRESS             = 2,
    SH7305_USB_DVSQ_CONFIGURED          = 3,
};
/* SH&SH7305_USB_REQT_* - device request value */
enum {
    SH7305_USB_REQT_GET_STATUS          = 0x00,
    SH7305_USB_REQT_CLEAR_FEATURE       = 0x01,
    SH7305_USB_REQT_SET_FEATURE         = 0x03,
    SH7305_USB_REQT_SET_ADDRESS         = 0x05,
    SH7305_USB_REQT_GET_DESCRIPTOR      = 0x06,
    SH7305_USB_REQT_SET_DESCRIPTOR      = 0x07,
    SH7305_USB_REQT_GET_CONFIGURATION   = 0x08,
    SH7305_USB_REQT_SET_CONFIGURATION   = 0x09,
};

/* sh7305_usb_hpowered() : check if the USB module is powered */
extern bool sh7305_usb_hpowered(void);

/* sh7305_usb_hpoweron() : power on the USB module */
extern int sh7305_usb_hpoweron(void);

/* sh7305_usb_hpoweroff() : power off the USB module */
extern int sh7305_usb_hpoweroff(void);


/* sh7305_usb_interrupt_handler() : interrupt handler invoked by hardware */
extern void sh7305_usb_interrupt_handler(void);

/* sh7305_usb_request_handle() : handle USB request */
extern void sh7305_usb_request_handle(void);

#include <vhex/usb/configuration.h>

/* sh7305_usb_open() : power on the module and setup USB module */
extern int sh7305_usb_open(struct usb_full_device *device);



/* sh7305_pipe_find() : try to find a pipe */
extern int sh7305_usb_pipe_find(int type);



/* sh7305_usb_dcp_prepare() : prepare FIFO buffer */
extern void sh7305_usb_dcp_prepare(void);

/* sh7305_usb_dcp_write() : send data through DCP pipe */
extern int sh7305_usb_dcp_write(void *buffer, size_t n);

/* sh7305_usb_dcp_commit() : send data */
extern void sh7305_usb_dcp_commit(void);


//---
// Utils
//---

#include <vhex/driver/mpu/sh/sh7305/tmu.h>
#include <vhex/usb/logger.h>
#include <vhex/defs/call.h>

/* SH7305_USB_REG_SYN() : activly wait the condition. */
#define SH7305_USB_REG_SYNC(condition) ({       \
    volatile int __trigger = 0;                 \
    int __timer = sh7305_tmu_configure(         \
        100000 /*µs*/,                          \
        VHEX_CALL_SET_STOP(&__trigger)          \
    );                                          \
    if(__timer >= 0) { timer_start(__timer); }  \
    while((condition) && __trigger == 0) {      \
        ;                                       \
    }                                           \
    sh7305_tmu_stop(__timer);                   \
    if(__trigger != 0) {                        \
        usb_logger_write(                       \
            "%s: %d: (" #condition ") holds\n", \
            __FUNCTION__,                       \
            __LINE__                            \
        );                                      \
    }                                           \
})

/* SH7305_USB_INTSTS0_CLEAR() : clear interrup flags */
#define SH7305_USB_INTSTS0_CLEAR(field_name) ({                     \
    __typeof__(SH7305_USB.INTSTS0) __intsts0 = { .word = 0xffff };  \
    __intsts0.field_name = 0;                                       \
    do SH7305_USB.INTSTS0 = __intsts0;                              \
    while(SH7305_USB.INTSTS0.field_name != 0);                      \
})

#endif /*__VHEX_MPU_SH7305_USB_ */
