#include <vhex/dma/interface.h>
#include <vhex/dma.h>

/* internal driver information */
extern struct {
    struct dma_drv_interface driver;
} dma_info;

/* dma_memcpy() : memcpy using DMA */
int dma_wait(dma_id_t id)
{
    if (dma_info.driver.dma_wait != NULL)
        return dma_info.driver.dma_wait(id);
    return -1;
}
