#include <vhex/dma/interface.h>
#include <vhex/dma.h>

/* internal driver information */
extern struct {
    struct dma_drv_interface driver;
} dma_info;

/* dma_memcpy() : memcpy using DMA */
dma_id_t dma_memcpy(void * restrict dst, void * restrict src, size_t sz)
{
    if (dma_info.driver.dma_memcpy != NULL)
        return dma_info.driver.dma_memcpy(dst, src, sz);
    return -1;
}
