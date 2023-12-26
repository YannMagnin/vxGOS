#ifndef VXGOS_BOOTLOADER_RASPI3B_AUXILIARY_H
#define VXGOS_BOOTLOADER_RASPI3B_AUXILIARY_H

#include "raspi3b/utils.h"

//---
// BCM2837 Auxiliary peripheral
//
// @note
// <> all of theses bits is based on the documentation on
//      https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf
// <> we use MSB instead of LSB packed struct because of the architecture
//      note that this cannot be modified with compiler flags.
//---

struct __bcm2837_aux
{
    /* [0x00215000] pending interrupt flags */
    const u32_union(AUX_IRQ,
        uint32_t MiniUART   :1;     /* Enable Mini UART */
        uint32_t SPI1       :1;     /* Enable SPI1 */
        uint32_t SPI2       :1;     /* Enable SPI2 */
        uint32_t            :29;
    );
    /* [0x00215004] enable auxialiary modules */
    u32_union(AUX_ENABLE,
        uint32_t MiniUART   :1;     /* Enable Mini UART */
        uint32_t SPI1       :1;     /* Enable SPI1 */
        uint32_t SPI2       :1;     /* Enable SPI2 */
        uint32_t            :29;
    );
    pad(0x38);

    //---
    // Mini UART register
    //---

    /* [0x00215040] used to write or write data from the FIFOs. */
    u32_union(AUX_MU_IO,
        uint32_t DATA       : 8;
        uint32_t            :24;
    );
    /* [0x00215044] used to enable interrupts */
    u32_union(AUX_MU_IER,
        uint32_t RIE        :1;     /* Receive Interrupt Enable */
        uint32_t TIE        :1;     /* Transmit Interrupt Enable */
        uint32_t            :30;
    );
    /* [0x00215048] Interrupt Status */
    u32_union(AUX_MU_IIR,
        uint32_t PENDING    :1;     /* =0 if pending interrupts */
        uint32_t TXFLUSH    :1;     /* Receiver holds valid byte */
        uint32_t RXFLUSH    :1;     /* Transmit holding register empty */
        uint32_t            :3;
        uint32_t FIFO       :2;     /* FIFO enabled (TODO: check RX/TX) */
        uint32_t            :24;
    );
    /* [0x0021504c] Control Line Data */
    u32_union(AUX_MU_LCR,
        uint32_t CHR        :2;     /* Character Length */
        uint32_t            :4;
        uint32_t BREAK      :1;     /* Pull down the UART1_TX line */
        uint32_t DLAB       :1;     /* Give access to the Baudrate reg */
        uint32_t            :24;
    );
    /* [0x00215050] Control the 'modem' signals */
    u32_union(AUX_MU_MCR,
        uint32_t            :1;
        uint32_t RTS        :1;     /* Pull down the UART1_RTS line */
        uint32_t            :30;
    );
    /* [0x00215054] Data Status */
    const u32_union(AUX_MU_LSR,
        uint32_t DRIE       :1;     /* Received Data Ready */
        uint32_t ORER       :1;     /* Overrrun Error */
        uint32_t            :3;
        uint32_t TDFE       :1;     /* Transmitter Empty */
        uint32_t TDFI       :1;     /* Transmitter Idle */
        uint32_t            :25;
    );
    /* [0x00215058] Data 'modem' status */
    const u32_union(AUX_MU_MSR,
        uint32_t            :4;
        uint32_t CTS        :1;     /* Inverse of the UART1_CTS input */
        uint32_t            :27;
    );
    /* [0x0021505c] Single Byte Storage */
    u32_union(AUX_MU_SCRATCH,
        uint32_t SCRATCH    :8;     /* SDC byte extra */
        uint32_t            :24;
    );
    /* [0x00215060] Access to Extra features */
    u32_union(AUX_MU_CNTL,
        uint32_t RE         :1;     /* Receiver Enable */
        uint32_t TE         :1;     /* Transmitter Enable */
        uint32_t ERAF       :1;     /* Enable Rx Auto Flow-control */
        uint32_t ETAF       :1;     /* Enable Tx Auto Flow-control */
        uint32_t RTS_AUTO   :2;     /* Rx Auto Flow Level */
        uint32_t RTS_ASSERT :1;     /* Rx RTS invert */
        uint32_t CTS_ASSERT :1;     /* CTS invert */
        uint32_t            :24;
    );
    /* [0x00215064] Internal Status */
    const u32_union(AUX_MU_STAT,
        uint32_t SYMA       :1;     /* Symbol Available */
        uint32_t SPCA       :1;     /* Tx can accept a least one byte */
        uint32_t RII        :1;     /* Rx is idle */
        uint32_t TII        :1;     /* Tx is idle */
        uint32_t ROVRE      :1;     /* Rx overrun */
        uint32_t TDFF       :1;     /* Tx FIFO is full */
        uint32_t RTS        :1;     /* status of the UART1_RTS line */
        uint32_t CTS        :1;     /* status of the UART1_CTS line */
        uint32_t TDFE       :1;     /* Tx FIFO is empty */
        uint32_t TEND       :1;     /* Tx End */
        uint32_t            :6;
        uint32_t RFFL       :4;     /* Rx FIFO fill level */
        uint32_t            :4;
        uint32_t TFFL       :4;     /* Tx FIFO fill level */
        uint32_t            :4;
    );
    /* [0x00215068] baudrate counter */
    u32_union(AUX_MU_BAUD,
        uint32_t BAUD       :16;    /* Baudrate */
        uint32_t            :16;
    );
} VPACKED(4);

#define BCM2837_AUXILIARY   (*((volatile struct __bcm2837_aux *)0x3f215000))

#endif /* VXGOS_BOOTLOADER_RASPI3B_AUXILIARY_H */
