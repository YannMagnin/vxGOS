#ifndef __VHEX_ARCH_SH7305_INTC__
# define __VHEX_ARCH_SH7305_INTC__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>
#include <vhex/defs/call.h>

/* sh7305_intc_ipc - Interrupt Priority Controller
   Some of the fields have been left unnamed because they correspond to SH7724
   peripheral modules that are *very* unlikely to even exist in the SH7305, let
   alone by of any use to us */
typedef volatile struct {
    word_union(IPRA,
        uint16_t TMU0_0     :4; /* TMU0 Channel 0 */
        uint16_t TMU0_1     :4; /* TMU0 Channel 1 */
        uint16_t TMU0_2     :4; /* TMU0 Channel 2 */
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRB,
        uint16_t ADC        :4; /* A/D Converter */
        uint16_t _1         :4; /* Unknown (TODO) */
        uint16_t _2         :4; /* Unknown (TODO) */
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRC,
        uint16_t            :4;
        uint16_t            :4;
        uint16_t            :4;
        uint16_t SPU        :4; /* SPU's DSP0 and DSP1 */
    );
    pad(2);

    word_union(IPRD,
        uint16_t            :4;
        uint16_t _MMCIF     :4; /* SH7724: MultiMedia Card Interface */
        uint16_t            :4;
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRE,
        uint16_t DMAC0A     :4; /* Direct Memory Access Controller 0 */
        uint16_t            :4;
        uint16_t ETMU3      :4; /* Extra TMU 3 */
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRF,
        uint16_t KEYSC      :4; /* Key Scan Interface */
        uint16_t DMACOB     :4; /* DMAC0 transfer/error info */
        uint16_t USB0_1     :4; /* USB controller */
        uint16_t CMT        :4; /* Compare Match Timer */
    );
    pad(2);

    word_union(IPRG,
        uint16_t _SCIF0     :4; /* SH7724: SCIF0 transfer/error info */
        uint16_t ETMU1      :4; /* Extra TMU 1 */
        uint16_t ETMU2      :4; /* Extra TMU 2 */
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRH,
        uint16_t _MSIOF0    :4; /* SH7724: Sync SCIF channel 0 */
        uint16_t _MSIOF1    :4; /* SH7724: Sync SCIF channel 1 */
        uint16_t _1         :4; /* Unknown (TODO) */
        uint16_t _2         :4; /* Unknown (TODO) */
    );
    pad(2);

    word_union(IPRI,
        uint16_t ETMU4      :4; /* Extra TMU 4 */
        uint16_t            :4;
        uint16_t _          :4; /* Unknown (TODO) */
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRJ,
        uint16_t ETMU0      :4; /* Extra TMU 0 */
        uint16_t _1         :4; /* Unknown (TODO) */
        uint16_t FSI        :4; /* FIFO-Buffered Serial Interface */
        uint16_t _2         :4; /* Unknown (TODO) */
    );
    pad(2);

    word_union(IPRK,
        uint16_t RTC        :4; /* Real-Time Clock */
        uint16_t SDC        :4; /* SD Card Controller */
        uint16_t            :4;
        uint16_t            :4;
    );
    pad(2);

    word_union(IPRL,
        uint16_t ETMU5      :4; /* Extra TMU 5 */
        uint16_t _          :4; /* Unknown (TODO) */
        uint16_t            :4;
        uint16_t            :4;
    );
} VPACKED(4) sh7305_intc_ipc_t;




/* sh7305_intc_masks_t - Interrupt mask management
   Writing 1 to IMR masks interrupts; writing 1 to IMCRs clears the masks.
   Writing 0 is ignored; reading from IMCRs yields undefined values */
typedef volatile struct {
    byte_union(IMR0,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
        uint8_t _2          :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR1,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t DEI3        :1; /* DMAC0 channel 3 */
        uint8_t DEI2        :1; /* DMAC0 channel 2 */
        uint8_t DEI1        :1; /* DMAC0 channel 1 */
        uint8_t DEI0        :1; /* DMAC0 channel 0 */
    );
    pad(3);

    byte_union(IMR2,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
        uint8_t ETUNI3      :1; /* ETMU3 */
    );
    pad(3);

    byte_union(IMR3,
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
        uint8_t _2          :1; /* Unknown (TODO) */
        uint8_t _3          :1; /* Unknown (TODO) */
        uint8_t DSP0        :1; /* SPU DSP1 */
        uint8_t DSP1        :1; /* SPU DSP0 */
        uint8_t             :1;
        uint8_t             :1;
    );
    pad(3);

    byte_union(IMR4,
        uint8_t             :1;
        uint8_t TUNI0       :1; /* TMU0 timer 0 */
        uint8_t TUNI1       :1; /* TMU0 timer 1 */
        uint8_t TUNI2       :1; /* TMU0 timer 2 */
        uint8_t ADC         :1; /* A/D Converter */
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _           :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR5,
        uint8_t KEYSC       :1; /* KEYSC */
        uint8_t DADERR      :1; /* DMAC0 Address error */
        uint8_t DEI5        :1; /* DMAC Channel 5 */
        uint8_t DEI4        :1; /* DMAC Channel 4 */
        uint8_t             :1;
        uint8_t ETMU2       :1; /* ETMU 2 */
        uint8_t ETMU1       :1; /* ETMU 1 */
        uint8_t _           :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR6,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t ETUNI4      :1; /* Extra Timer 4 */
        uint8_t ETUNI0      :1; /* Extra Timer 0 */
        uint8_t             :1;
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR7,
        uint8_t DTE0I       :1; /* I2C0 Transmit */
        uint8_t WAITT0I     :1; /* I2C0 Wait */
        uint8_t TACK0I      :1; /* I2C0 Non-ACK */
        uint8_t AL0I        :1; /* I2C0 Arb. lost */
        uint8_t TEINTE      :1; /* FCNTL Transfer end (SH7780)*/
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
        uint8_t _2          :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR8,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t ETUNI5      :1; /* Extra Timer 5 */
        uint8_t FSI         :1; /* Fifo Serial Interface */
    );
    pad(3);

    byte_union(IMR9,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t CMT         :1; /* CMT */
        uint8_t             :1;
        uint8_t             :1;
        uint8_t USI         :1; /* USB */
        uint8_t             :1;
    );
    pad(3);

    byte_union(IMR10,
        uint8_t const       :1;
        uint8_t const       :1;
        uint8_t SDC         :1; /* SD Card */
        uint8_t _           :1; /* Unknown (TODO) */
        uint8_t             :1;
        uint8_t ATI         :1; /* RTC alarm */
        uint8_t PRI         :1; /* RTC periodic interrupt */
        uint8_t CUI         :1; /* RTC carry */
    );
    pad(3);

    byte_union(IMR11,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _           :1; /* Unknown (TODO) */
    );
    pad(3);

    byte_union(IMR12,
        uint8_t             :1;
        uint8_t             :1;
        uint8_t _0          :1; /* Unknown (TODO) */
        uint8_t _1          :1; /* Unknown (TODO) */
        uint8_t _2          :1; /* Unknown (TODO) */
        uint8_t             :1;
        uint8_t             :1;
        uint8_t             :1;
    );
} VPACKED(4) sh7305_intc_masks_t;




/* sh7305_intc_userimask_t - User Interrupt Mask
   Sets the minimum required level for interrupts to be accepted.

   WARNING: Writing to this register is only allowed when the upper bits of the
   operand (ie. the new value of USERIMASK) are 0xa5; otherwise, the write is
   ignored. To modify the value of this register, do not access the bit field
   directly, backup the variable and modify it:

    void set_user_imask(int new_level)
    {
        sh7305_intc_userimask_t mask = *(INTC._7305.USERIMASK);
        mask._0xa5  = 0xa5;
        mask.UIMASK = new_level & 0x0f;
        *(INTC._7305.USERIMASK) = mask;
    }
*/
typedef volatile lword_union(sh7305_intc_userimask_t,
    uint32_t _0xa5          :8; /* Always set to 0xa5 before writing */
    uint32_t                :16;
    uint32_t UIMASK         :4; /* User Interrupt Mask Level */
    uint32_t                :4;
);




/* sh7305_icr0_t - Interrupt Control register 0
   Configure the input signal detection mode. Note that this register differ a
   bit from the SH7724 because the LVLMODE bit which allow us to select the
   interrupt source retention seems removed in the SH7305 (emulator) */
typedef volatile word_union(sh7305_intc_icr0_t,
    uint16_t NMIL           :1; /* NMI Input level */
    uint16_t MAI            :1; /* NMI Interrupt Mask */
    uint16_t                :4;
    uint16_t NMIB           :1; /* NMI Block Mode */
    uint16_t NMIE           :1; /* NMI Edge Select */
    uint16_t _HIGH          :2; /* should be 1 */
    uint16_t                :6;
);

/* sh7305_icr1_t - Interrupt Control register 1
   Configure the detection mode for the external interrupt pins IRQ0 to IRQ3.
   Note that the register differ from the SH7724 because we don't have the IRQ4,
   IRQ5, IRQ6 and IRQ7. */
typedef volatile word_union(sh7305_intc_icr1_t,
    uint16_t IRQ0S          :2; /* IRQ0 Sense select */
    uint16_t IRQ1S          :2; /* IRQ1 Sense select */
    uint16_t IRQ2S          :2; /* IRQ2 Sense select */
    uint16_t IRQ3S          :2; /* IRQ3 Sense select */
    uint16_t                :8;
);

/* sh7305_intpri00_t - Interrupt Priority register 0
   Configure the interrupt level for the signal from pins IRQ0 to IRQ3. Note
   that the register differ from the SH7724 because we don't have IRQ4 to IQ7.*/
typedef volatile lword_union(sh7305_intc_intpri00_t,
    uint32_t IRQ0           :4; /* IRQ0 priority level */
    uint32_t IRQ1           :4; /* IRQ0 priority level */
    uint32_t IRQ2           :4; /* IRQ0 priority level */
    uint32_t IRQ3           :4; /* IRQ0 priority level */
    uint32_t                :16;
);

/* sh7305_intc_intreq00_t - Interrupt request 0 */
typedef volatile byte_union(sh7305_intc_intreq00_t,
    uint8_t IRQ0            :1; /* IRQ0 Interrupt Request */
    uint8_t IRQ1            :1; /* IRQ0 Interrupt Request */
    uint8_t IRQ2            :1; /* IRQ0 Interrupt Request */
    uint8_t IRQ3            :1; /* IRQ0 Interrupt Request */
    uint8_t                 :4;
);

/* sh7305_intc_intmsk00_t - Interrupt mask 0 */
typedef volatile byte_union(sh7305_intc_intmsk00_t,
    uint8_t IRQ0            :1; /* IRQ0 Interrupt Mask */
    uint8_t IRQ1            :1; /* IRQ0 Interrupt Mask */
    uint8_t IRQ2            :1; /* IRQ0 Interrupt Mask */
    uint8_t IRQ3            :1; /* IRQ0 Interrupt Mask */
    uint8_t                 :4;
);

/* sh7305_intc_intmskclr00_t - Interrupt mask clear 0 */
typedef volatile byte_union(sh7305_intc_intmskclr00_t,
    uint8_t IRQ0            :1; /* IRQ0 Interrupt Mask Clear */
    uint8_t IRQ1            :1; /* IRQ0 Interrupt Mask Clear */
    uint8_t IRQ2            :1; /* IRQ0 Interrupt Mask Clear */
    uint8_t IRQ3            :1; /* IRQ0 Interrupt Mask Clear */
    uint8_t                 :4;
);

/* sh7305_intc_nmifcr_t - NMI Flags control register
   This register contains the NMI interrupt flags that can be cleared. Note that
   the emultor set the NMIFL bit in NMIL. */
typedef volatile word_union(sh7305_intc_nmifcr_t,
    uint16_t NMIL           :1; /* NMI Input Level */
    uint16_t                :14;
    uint16_t NMIFL          :1; /* NMI Interrupt Request */
);



/* sh7305_intc - the SH7305 interrupt controller */
struct sh7305_intc
{
    /* Interrupt control register */
    sh7305_intc_icr0_t *ICR0;

    /* external IRQ pins configuration */
    sh7305_intc_icr1_t *ICR1;
    sh7305_intc_intpri00_t *INTPRI00;
    sh7305_intc_intreq00_t *INTREQ00;
    sh7305_intc_intmsk00_t *INTMSK00;
    sh7305_intc_intmskclr00_t *INTMSKCLR00;

    /* NMI configuration / flags */
    sh7305_intc_nmifcr_t *NMIFCR;

    /* Interrupt priority registers */
    union {
        sh7305_intc_ipc_t   *_IPRX;
        volatile uint16_t   *IPR;
    };

    /* Interrupt mask & mask clear registers */
    union {
        sh7305_intc_masks_t *_MSK;
        uint8_t             *MSK;
    };
    union {
        sh7305_intc_masks_t *_MSKCLR;
        uint8_t             *MSKCLR;
    };

    /* Other registers */
    sh7305_intc_userimask_t *USERIMASK;

} VPACKED(4);

/* Provided by intc/intc.c */
extern struct sh7305_intc SH7305_INTC;


//---
//  Interrupt names
//---

/* Because of SH3/SH4 differences and the different ways interrupts are mapped
   to bits of IPR and IMR registers, we define the following names for
   interrupt signals, and their corresponding IPR and IMR bits are defined in
   the INTC driver. */
enum {
    /* Timer Unit [TMU] */
    INTC_TMU_TUNI0      = 0,
    INTC_TMU_TUNI1      = 1,
    INTC_TMU_TUNI2      = 2,
    /* Extra Timer Unit [TMU]; SH7305-only except for ETMU_TUNI0 */
    INTC_ETMU_TUNI0     = 3,
    INTC_ETMU_TUNI1     = 4,
    INTC_ETMU_TUNI2     = 5,
    INTC_ETMU_TUNI3     = 6,
    INTC_ETMU_TUNI4     = 7,
    INTC_ETMU_TUNI5     = 8,
    /* DMA Controller [DMA0]; a single IPR is used for DEI0..3, and another
       IPR is used for DEI4..5 and DADERR together */
    INTC_DMA_DEI0       = 9,
    INTC_DMA_DEI1       = 10,
    INTC_DMA_DEI2       = 11,
    INTC_DMA_DEI3       = 12,
    INTC_DMA_DEI4       = 13,
    INTC_DMA_DEI5       = 14,
    INTC_DMA_DADERR     = 15,
    /* Real-Time Clock [RTC]; a single IPR covers all 3 interrupts */
    INTC_RTC_ATI        = 16,
    INTC_RTC_PRI        = 17,
    INTC_RTC_CUI        = 18,
    /* SPU; interrupts from the DSPs and the SPU-bound DMA */
    INTC_SPU_DSP0       = 19,
    INTC_SPU_DSP1       = 20,
    /* USB communication */
    INTC_USB            = 21,
    /* Kay Scan Interface [KEYSC]; SH7305-only */
    INTC_KEYSC          = 22,
};

//---
// hardware-specific primitives
//---

/* sh7305_intc_install_inth() : install interrupt gate */
extern void *sh7305_intc_install_inth_gate(int blockid, void *gate, size_t size);

/* sh7305_intc_priority() : set the interrupt name priority */
extern int sh7305_intc_priority(int intname, int level);

/* sh7305_intc_generic_handler() : install generic interrupt handler */
extern void *sh7305_intc_install_inth_generic(int event_code, vhex_call_t cb);

#endif /* __VHEX_ARCH_SH7305_INTC__ */
