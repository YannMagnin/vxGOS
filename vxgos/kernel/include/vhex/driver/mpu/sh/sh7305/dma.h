#ifndef __VHEX_DRIVER_MPU_SH_SH7305_DMA__
# define __VHEX_DRIVER_MPU_SH_SH7305_DMA__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

#include <vhex/dma/types.h>

//---
//  SH7305 Direct Memory Access Controller. Refer to:
//    "Renesas SH7724 User's Manual: Hardware"
//    Section 16: "Direct Memory Access Controller (DMAC)"
//
//  However, there is some (minor) differencies with the DMA controller of
//  the SH7305. Mainly, only the first channel have special features that
//  other don't.
//---

/* sh7305_dma_channel_t - One of the main 6 channels of the DMA

   Note that the many settings are only available on channels 0 to 3 (denoted
   by [0..3]) or on channels 0 (denoted by [0]).
   The documentation is apparently wrong about the placement is TS[3:2], the
   neighboring read-only bit must be swapped before TS[3:2]. */
struct __sh7305_dma_channel
{
    uint32_t SAR;
    uint32_t DAR;

    /* Mind that the 8 upper bits should always be written as 0 */
    uint32_t TCR;

    lword_union(CHCR,
        uint32_t        :1;
        uint32_t LCKN   :1; /* Bus Right Release Enable */
        uint32_t        :2;

        uint32_t RPT    :3; /* Repeat setting [0..3] */
        uint32_t DA     :1; /* DREQ Asynchronous [0] */

        uint32_t DO     :1; /* DMA Overrun  [0] */
        uint32_t        :1;
        uint32_t TS3    :1; /* Transfer Size (bit3) */
        uint32_t TS2    :1; /* Transfer Size (bit2) */

        uint32_t HE     :1; /* Half-End flag [0..3] */
        uint32_t HIE    :1; /* Half-end Interrupt Enable [0..3] */
        uint32_t AM     :1; /* Acknowledge mode  [0] */
        uint32_t AL     :1; /* Acknowledge level [0] */

        uint32_t DM     :2; /* Destination address Mode */
        uint32_t SM     :2; /* Source address Mode */

        uint32_t RS     :4; /* Resource Select */

        uint32_t DL     :1; /* DREQ Level [0] */
        uint32_t DS     :1; /* DREQ Source select[0] */
        uint32_t TB     :1; /* Transfer Bus Mode */
        uint32_t TS1    :1; /* Transfer Size (bit1) */

        uint32_t TS0    :1; /* Transfer Size (bit0) */
        uint32_t IE     :1; /* Interrupt Enable */
        uint32_t TE     :1; /* Transfer End flag */
        uint32_t DE     :1; /* DMA Enable */
    );

} VPACKED(4);

/* sh7305_dma_t - DMA Controller */
struct __sh7305_dma
{
    struct __sh7305_dma_channel DMA0;
    struct __sh7305_dma_channel DMA1;
    struct __sh7305_dma_channel DMA2;
    struct __sh7305_dma_channel DMA3;

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
    pad(14);

    struct __sh7305_dma_channel DMA4;
    struct __sh7305_dma_channel DMA5;

} VPACKED(4);

#define SH7305_DMA (*((volatile struct __sh7305_dma *)0xfe008020))

//---
// Kernel-level API
//---

/* dma_size_t - Transfer block size */
typedef enum
{
    /* Normal transfers of 1, 2, 4, 8, 16 or 32 bytes at a time */
    DMA_BLOCK_SIZE_1B       = 0,
    DMA_BLOCK_SIZE_2B       = 1,
    DMA_BLOCK_SIZE_4B       = 2,
    DMA_BLOCK_SIZE_8B       = 7,
    DMA_BLOCK_SIZE_16B      = 3,
    DMA_BLOCK_SIZE_32B      = 4,

    /* Transfers of 16 or 32 bytes divided in two operations */
    DMA_BLOCK_SIZE_16B_DIV  = 11,
    DMA_BLOCK_SIZE_32B_DIV  = 12,

} dma_block_size_t;

/* dma_addr_op_t - Addressing mode for source and destination regions */
typedef enum
{
    /* Fixed address mode: the same address is read/written at each step */
    DMA_ADDR_FIXED          = 0,
    /* Increment: address is incremented by transfer size at each step */
    DMA_ADDR_INC            = 1,
    /* Decrement: only allowed for 1/2/4-byte transfers */
    DMA_ADDR_DEC            = 2,
    /* Fixed division mode: address is fixed even in 16/32-divide mode */
    DMA_ADDR_FIXEDDIV       = 3,

} dma_addr_op_t;

/* sh7305_dma_id_t : internal DMA ID structure */
typedef union {
    uint32_t lword;
    struct {
        uint32_t INVALID    :1;
        uint32_t IDX        :3;
        uint32_t KEYFRAME   :28;
    };
} sh7305_dma_id_t;

/* internal cache channel information */
typedef struct {
    sh7305_dma_id_t id;
    volatile struct __sh7305_dma_channel *hw;
} sh7305_dma_channel_t;

extern dma_id_t sh7305_dma_transfert_async(
    uintptr_t src, dma_addr_op_t src_op,
    uintptr_t dst, dma_addr_op_t dst_op,
    dma_block_size_t block_size,
    int block_count
);

/* dma_memcpy() : memcpy using DMA */
extern dma_id_t sh7305_dma_memcpy(void * restrict, void * restrict, size_t);

/* dma_memset() : memset using the DMA */
extern dma_id_t sh7305_dma_memset(void *dst, int c, size_t sz);

/* dma_wait() : wait the end of the DMA channel transfer */
extern int sh7305_dma_wait(dma_id_t id);

/* dma_wait() : wait the end of the DMA channel transfer without interruption */
extern int sh7305_dma_spinwait(dma_id_t id);

/* sh7305_dma_wait() : wait DAM channel */
extern int sh7305_dma_channel_wait(sh7305_dma_channel_t *ch, int atomic);

/* sh7305_dma_channel_find() : find a channel using the ID */
extern sh7305_dma_channel_t *sh7305_dma_channel_find(sh7305_dma_id_t id);

#endif /* __VHEX_DRIVER_MPU_SH_SH7305_DMA__ */
