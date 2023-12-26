#include <vhex/dma/interface.h>
#include <vhex/dma.h>

/* internal driver information */
extern struct {
    struct dma_drv_interface driver;
} dma_info;

/* dma_memcpy() : memcpy using DMA */
dma_id_t dma_memset(void * restrict dst, int c, size_t sz)
{
    if (dma_info.driver.dma_memset != NULL)
        return dma_info.driver.dma_memset(dst, c, sz);
    return -1;
}
