#include <vhex/driver/mpu/sh/sh7305/dma.h>
#include <vhex/dma.h>

#include <string.h>

/* dma_memcpy() : memcpy using DMA */
dma_id_t sh7305_dma_memcpy(void * restrict dst, void * restrict src, size_t sz)
{
    uintptr_t _dst;
    uintptr_t _src;
    uintptr_t dma_src;
    uintptr_t dma_dst;
    ptrdiff_t delta;
    int dma_block_count;
    dma_block_size_t dma_block_size;
    int divisor;
    ptrdiff_t gap_size_pre;
    ptrdiff_t gap_size_end;
    dma_id_t id;

    if (sz < 64) {
        memcpy(dst, src, sz);
        return 0;
    }

    _dst = (uintptr_t)dst;
    _src = (uintptr_t)src;

    delta = (dst > src) ? dst - src : src - dst;
    if ((delta & 0x0000001f) == 0) {
        dma_block_size = DMA_BLOCK_SIZE_32B;
        divisor = 32;
    } else if ((delta & 0x0000000f) == 0) {
        dma_block_size = DMA_BLOCK_SIZE_16B;
        divisor = 16;
    } else if ((delta & 0x00000007) == 0) {
        dma_block_size = DMA_BLOCK_SIZE_8B;
        divisor = 8;
    } else if ((delta & 0x00000003) == 0) {
        dma_block_size = DMA_BLOCK_SIZE_4B;
        divisor = 4;
    } else if ((delta & 0x00000001) == 0) {
        dma_block_size = DMA_BLOCK_SIZE_2B;
        divisor = 2;
    } else {
        dma_block_size = DMA_BLOCK_SIZE_1B;
        divisor = 1;
    }

    dma_src = ((_src + divisor - 1) / divisor) * divisor;
    dma_dst = ((_dst + divisor - 1) / divisor) * divisor;
    gap_size_pre = dma_dst - _dst;
    dma_block_count = (sz - gap_size_pre) / divisor;
    gap_size_end = (sz - gap_size_pre) - (dma_block_count * divisor);

    id = sh7305_dma_transfert_async(
        dma_src, DMA_ADDR_INC,
        dma_dst, DMA_ADDR_INC,
        dma_block_size,
        dma_block_count
    );
    if (id < 0)
        return id;

    for (int i = 0; i < gap_size_pre; ++i) {
        ((uint8_t*)_dst)[0] = ((uint8_t*)_src)[0];
        _dst += 1;
        _src += 1;
    }
    _dst += dma_block_count * divisor;
    _src += dma_block_count * divisor;
    for (int i = 0; i < gap_size_end; ++i) {
        ((uint8_t*)_dst)[0] = ((uint8_t*)_src)[0];
        _dst += 1;
        _src += 1;
    }

    return id;
}
