#include <vhex/driver/mpu/sh/sh7305/dma.h>
#include <vhex/driver/mpu/sh/sh7305/intc.h>
#include <vhex/driver/mpu/sh/sh7305/power.h>
#include <vhex/driver/cpu.h>
#include <vhex/driver.h>
#include <vhex/defs/call.h>
#include <vhex/dma/interface.h>

#include <vhex/display.h>


/* internal DMA cache information */
static struct {
    uint32_t keyframe;
    sh7305_dma_channel_t channel[6];
} dmac_info = {
    .keyframe = 0x00000000,
    .channel = {
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA0
        },
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA1
        },
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA2
        },
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA3
        },
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA4
        },
        {
            .id = {
                .INVALID  = 1,
                .IDX      = 0b111,
                .KEYFRAME = 0
            },
            .hw = &SH7305_DMA.DMA5
        },
    }
};

//---
// Driver internals
//---

/* dma_translate(): Translate virtual address to DMA-suitable form */
static uintptr_t sh7305_dma_translate_addr(uintptr_t a)
{
    /* Preserve RS addresses (as of SH7724 Reference, 11.2.2) */
    if(a >= 0xfd800000 && a < 0xfd800800)
        return a;

    /* Translate virtual addresses to IL memory to physical addresses; the
       same address is used (as of SH7724 Reference, 10.3.3) */
    if(a >= 0xe5200000 && a < 0xe5204000)
        return a;

    /* First additional on-chip memory area (XRAM) */
    if(a >= 0xe5007000 && a < 0xe5009000)
        return a;

    /* Second on-chip memory area (YRAM) */
    if(a >= 0xe5017000 && a < 0xe5019000)
        return a;

    /* Translate P1 and P2 addresses to ROM and RAM to physical form */
    if(a >= 0x80000000 && a < 0xc0000000)
        return a & 0x1fffffff;

    /* By default: I don't know what this is, let's preserve it */
    return a;
}

/* sh7305_dma_channel_reserve() : wait until a channel is free and reserve it */
static sh7305_dma_channel_t *sh7305_dma_channel_reserve(void)
{
    sh7305_dma_channel_t *ch;

    ch = NULL;
    while (1)
    {
        cpu_atomic_start();
        for (int i = 5; i >= 0; --i) {
            if (dmac_info.channel[i].id.INVALID == 0)
                continue;
            dmac_info.keyframe += 1;
            ch = &dmac_info.channel[i];
            ch->id.INVALID = 0;
            ch->id.IDX = i;
            ch->id.KEYFRAME = dmac_info.keyframe;
            break;
        }
        cpu_atomic_end();

        if (ch != NULL)
            break;

        __asm__ volatile ("sleep");
    }

    return ch;
}

#if 0
/* sh7305_dma_channel_release() : release DMA channel */
static int sh7305_dma_channel_release(sh7305_dma_channel_t * ch, int atomic)
{
    if (sh7305_dma_channel_wait(ch, atomic) != 0)
        return -1;

    ch->id.INVALID = 1;
    return 0;
}
#endif

/* sh7305_dma_channel_find() : find a channel using the ID */
sh7305_dma_channel_t *sh7305_dma_channel_find(sh7305_dma_id_t id)
{
    if (id.INVALID == 1)
        return NULL;
    if (id.IDX < 0 && id.IDX >= 6)
        return NULL;
    return &dmac_info.channel[id.IDX];
}

/* sh7305_dma_wait() : wait DAM channel */
int sh7305_dma_channel_wait(sh7305_dma_channel_t *ch, int atomic)
{
    int find;

    find = -1;

    cpu_atomic_start();
    for (int i = 0; i < 6; ++i) {
        if (ch != &dmac_info.channel[i])
            continue;
        find = 0;
        break;
    }
    cpu_atomic_end();

    if (find != 0)
        return find;

    while (1) {
        if (ch->hw->CHCR.DE == 0)
            break;
        if (atomic == 1)
            __asm__ volatile ("sleep");
    }

    return 0;
}



//---
// Driver public API
//---

/* sh7305_dma_transfert_async() : start asynchronous transfert */
dma_id_t sh7305_dma_transfert_async(
    uintptr_t src, dma_addr_op_t src_op,
    uintptr_t dst, dma_addr_op_t dst_op,
    dma_block_size_t block_size,
    int block_count
) {
    sh7305_dma_channel_t *ch;

    ch = sh7305_dma_channel_reserve();
    if (ch == NULL)
        return -1;

    /* disable channel */
    ch->hw->CHCR.DE = 0;

    /* set source / destination address and block counter */
    ch->hw->SAR = sh7305_dma_translate_addr(src);
    ch->hw->DAR = sh7305_dma_translate_addr(dst);
    ch->hw->TCR = block_count & 0x0fffffff;

    /* common channel configuration
       <> enable release of the bus mastership between reading and writting
       <> select normal mode (channel 0..3)
       <> select asynchronous DREQ sampling (channel 0)
       <> detect overrun 0 (channel 0)
       <> clear half-end flags
       <> disable half-end interruption
       <> select DACK output at read cycle (channel 0)
       <> select low-active output of DACK (channel 0)
       <> set the approriate destination addressing mode
       <> set the approriate source addressing mode
       <> select auto-request transfert
       <> select DREQ detection on low-level (channel 0)
       <> select cycle steal mode
       <> enable interruption
       <> clear interrupt flags */
    ch->hw->CHCR.LCKN = 1;
    ch->hw->CHCR.RPT = 0b000;
    ch->hw->CHCR.DA  = 0;
    ch->hw->CHCR.DO  = 0;
    ch->hw->CHCR.HE  = 0;
    ch->hw->CHCR.HIE = 0;
    ch->hw->CHCR.AM  = 0;
    ch->hw->CHCR.AL  = 0;
    ch->hw->CHCR.DM = dst_op;
    ch->hw->CHCR.SM = src_op;
    ch->hw->CHCR.RS = 0b0100;
    ch->hw->CHCR.DL = 0;
    ch->hw->CHCR.DS = 0;
    ch->hw->CHCR.TB = 0;
    ch->hw->CHCR.IE = 1;
    ch->hw->CHCR.TE = 0;

    /* set the block size */
    ch->hw->CHCR.TS3 = (block_size & 0b1000) >> 3;
    ch->hw->CHCR.TS2 = (block_size & 0b0100) >> 2;
    ch->hw->CHCR.TS1 = (block_size & 0b0010) >> 1;
    ch->hw->CHCR.TS0 = (block_size & 0b0001) >> 0;

    /* start DMA channel */
    ch->hw->CHCR.DE = 1;

    return ch->id.lword;
}

//---
// Kernel-level API
//---

/* Interrupt handler for all finished DMA transfers */
static void sh7305_dma_interrupt_transfer_ended(sh7305_dma_channel_t *ch)
{
    ch->hw->CHCR.IE = 0;
    ch->hw->CHCR.DE = 0;
    ch->hw->CHCR.TE = 0;

    SH7305_DMA.DMAOR.AE = 0;
    SH7305_DMA.DMAOR.NMIF = 0;

    ch->id.INVALID = 1;
}

/* DMA error handler (interrupt) */
static void sh7305_dma_error_handler(void)
{
    dclear(C_WHITE);
    dprint(0, 0, C_BLACK,
        "DMA error handler !!!\n"
        "  DMA0.CHCR = %p\n"
        "  DMA1.CHCR = %p\n"
        "  DMA2.CHCR = %p\n"
        "  DMA3.CHCR = %p\n"
        "  DMA4.CHCR = %p\n"
        "  DMA5.CHCR = %p\n"
        "  DMAOR = %p\n"
        "reset the device..."
        ,
        SH7305_DMA.DMA0.CHCR.lword,
        SH7305_DMA.DMA1.CHCR.lword,
        SH7305_DMA.DMA2.CHCR.lword,
        SH7305_DMA.DMA3.CHCR.lword,
        SH7305_DMA.DMA4.CHCR.lword,
        SH7305_DMA.DMA5.CHCR.lword,
        SH7305_DMA.DMAOR.word
    );
    dupdate();
    while (1) { __asm__ volatile ("sleep"); }
}

//---
// Declare driver
//---

//FIXME: avoid manualy sync DMAOR with header o(x_x)o
struct dma_ctx {
    volatile struct __sh7305_dma_channel ch[6];
    word_union(DMAOR,
        uint16_t CMS    :4; /* Cycle steal Mode Select */

        uint16_t        :2;
        uint16_t PR     :2; /* PRiority mode */

        uint16_t        :4;

        uint16_t        :1;
        uint16_t AE     :1; /* Address Error flag */
        uint16_t NMIF   :1; /* NMI Flag */
        uint16_t DME    :1; /* DMA Master Enable */
    );
};

static bool __dma_hpowered(void)
{
    return (SH7305_POWER.MSTPCR0.DMA == 0);
}

static int __dma_hpoweron(void)
{
    SH7305_POWER.MSTPCR0.DMA = 0;
    return 0;
}

static int __dma_hpoweroff(void)
{
    SH7305_POWER.MSTPCR0.DMA = 1;
    return 0;
}

static void __dma_configure(struct dma_ctx *s)
{
    extern void sh7305_dma_inth_te(void);
    extern void sh7305_dma_inth_ae(void);

    int codes[] = { 0x800, 0x820, 0x840, 0x860, 0xb80, 0xba0 };

    for(int i = 0; i < 6; i++)
    {
        sh7305_intc_install_inth_generic(
            codes[i],
            VHEX_CALL(
                &sh7305_dma_interrupt_transfer_ended,
                (void*)&dmac_info.channel[i]
            )
        );

        s->ch[i].SAR = 0x00000000;
        s->ch[i].DAR = 0x00000000;
        s->ch[i].TCR = 0x00000000;

        s->ch[i].CHCR.DE = 0;

        dmac_info.channel[i].id.INVALID = 1;
    }

    /* Configure the DMA operations
       <> select normal cycle steal mode
       <> select round-robin mode
       <> clear address error flags
       <> clear NMIF flags
       <> enable the master switch */
    s->DMAOR.CMS  = 0b0000;
    s->DMAOR.PR   = 0b11;
    s->DMAOR.AE   = 0;
    s->DMAOR.NMIF = 0;
    s->DMAOR.DME  = 1;

    /* Install Address Error interrupt gate */
    sh7305_intc_install_inth_generic(
        0xbc0,
        VHEX_CALL(&sh7305_dma_error_handler)
    );

    /* Set interrupt priority to 3, except for the channels that are
       used by the USB driver (channel 0) */
    sh7305_intc_priority(INTC_DMA_DEI0, 9);
    sh7305_intc_priority(INTC_DMA_DEI1, 3);
    sh7305_intc_priority(INTC_DMA_DEI2, 3);
    sh7305_intc_priority(INTC_DMA_DEI3, 3);
    sh7305_intc_priority(INTC_DMA_DEI4, 3);
    sh7305_intc_priority(INTC_DMA_DEI5, 3);
    sh7305_intc_priority(INTC_DMA_DADERR, 3);
}

static void __dma_hsave(struct dma_ctx *s)
{
    sh7305_dma_channel_t * ch;

    for(int i = 0; i < 6; i++) {
        ch = &dmac_info.channel[i];
        sh7305_dma_channel_wait(ch, 1);
        s->ch[i].SAR        = ch->hw->SAR;
        s->ch[i].DAR        = ch->hw->DAR;
        s->ch[i].TCR        = ch->hw->TCR;
        s->ch[i].CHCR.lword = ch->hw->CHCR.lword;
    }

    s->DMAOR.word = SH7305_DMA.DMAOR.word;
}

static void __dma_hrestore(struct dma_ctx *s)
{
    sh7305_dma_channel_t * ch;

    SH7305_DMA.DMAOR.DME = 0;

    for(int i = 0; i < 6; i++)
    {
        ch = &dmac_info.channel[i];
        ch->hw->SAR        = s->ch[i].SAR;
        ch->hw->DAR        = s->ch[i].DAR;
        ch->hw->TCR        = s->ch[i].TCR;
        ch->hw->CHCR.lword = s->ch[i].CHCR.lword;
    }

    SH7305_DMA.DMAOR.word = s->DMAOR.word;
}

struct vhex_driver drv_dma = {
    .name       = "DMA",
    .hpowered   = (void*)&__dma_hpowered,
    .hpoweron   = (void*)&__dma_hpoweron,
    .hpoweroff  = (void*)&__dma_hpoweroff,
    .hsave      = (void*)&__dma_hsave,
    .hrestore   = (void*)&__dma_hrestore,
    .configure  = (void*)&__dma_configure,
    .state_size = sizeof(struct dma_ctx),
    .flags = {
        .DMA      = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct dma_drv_interface){
        .dma_memcpy   = &sh7305_dma_memcpy,
        .dma_memset   = &sh7305_dma_memset,
        .dma_wait     = &sh7305_dma_wait,
        .dma_spinwait = &sh7305_dma_spinwait
    }
};
VHEX_DECLARE_DRIVER(05, drv_dma);
