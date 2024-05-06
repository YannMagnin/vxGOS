#include <vhex/driver/mpu/sh/sh7305/dma.h>
#include <vhex/dma.h>

#include <string.h>

/* Allocate a 32-byte buffer in ILRAM */
VALIGNED(32) VSECTION(".xram") static uint32_t Xbuf[8];

/* dma_memset() : memset using the DMA */
dma_id_t sh7305_dma_memset(void *dst, int c, size_t sz)
{
    uintptr_t dma_src;
    uintptr_t dma_dst;
    ptrdiff_t gap_size_pre;
    ptrdiff_t gap_size_end;
    uint32_t mask;
    int dma_block_count;
    dma_id_t id;

    if (sz < 64) {
        memset(dst, c, sz);
        return 0;
    }

    mask = (
          ((c & 0xff) << 24)
        | ((c & 0xff) << 16)
        | ((c & 0xff) << 8)
        | ((c & 0xff) << 0)
    );

    /* Prepare the XRAM buffer. We need to use XRAM because the DMA will
       have to read the operand once per block, as opposed to an assembler
       routine that would hold it in a register. If we place it in RAM, the
       DMA will perform twice as many RAM accesses as the handwritten
       assembler, which would be very slow. By using XRAM we use two
       different memory regions, making the DMA faster than the CPU. */
    for(int i = 0; i < 8; i++) { Xbuf[i] = mask; }

    //FIXME: SPU

    dma_src = (uintptr_t)Xbuf;
    dma_dst = (uintptr_t)dst & 0xffffffe0;
    gap_size_pre = dma_dst - (uintptr_t)dst;
    dma_block_count = (sz - gap_size_pre) >> 5;
    gap_size_end = (sz - gap_size_pre) - (dma_block_count << 5);

    id = sh7305_dma_transfert_async(
        dma_src, DMA_ADDR_FIXED,
        dma_dst, DMA_ADDR_INC,
        DMA_BLOCK_SIZE_32B,
        dma_block_count
    );
    if (id < 0)
        return id;

    for (int i = 0; i < gap_size_pre; ++i) {
        ((uint8_t*)dst)[0] = c;
        dst = &((uint8_t*)dst)[1];
    }
    dst += dma_block_count << 5;
    for (int i = 0; i < gap_size_end; ++i) {
        ((uint8_t*)dst)[0] = c;
        dst = &((uint8_t*)dst)[1];
    }

    return id;
}
