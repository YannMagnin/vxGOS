#ifndef __VHEX_DMA_INTERFACE__
# define __VHEX_DMA_INTERFACE__

#include <vhex/dma/types.h>

struct dma_drv_interface
{
    dma_id_t (*dma_memcpy)(void * restrict, void * restrict, size_t);
    dma_id_t (*dma_memset)(void * restrict, int, size_t);
    int (*dma_wait)(dma_id_t);
    int (*dma_spinwait)(dma_id_t);
};

#endif /* __VHEX_DMA_INTERFACE__ */
