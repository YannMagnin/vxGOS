#include <vhex/driver.h>
#include <vhex/dma/interface.h>
#include <vhex/dma.h>

#include <string.h>
#include <stdio.h>

/* dma_memcpy() : memcpy using DMA */
dma_id_t sdl_dma_memcpy(void * restrict dst, void * restrict src, size_t sz)
{
    memcpy(dst, src, sz);
    return 0;
}

/* dma_memset() : memset using the DMA */
dma_id_t sdl_dma_memset(void *dst, int c, size_t sz)
{
    memset(dst, c, sz);
    return 0;
}

/* dma_wait() : wait the end of the DMA channel transfer */
int sdl_dma_wait(dma_id_t id)
{
    (void)id;
    return 0;
}

/* dma_wait() : wait the end of the DMA channel transfer without interruption */
int sdl_dma_spinwait(dma_id_t id)
{
    (void)id;
    return 0;
}

//---
// Fake driver definition
//---

static void __dma_configure(void)
{
    // Nothing to do, this is a fake driver
    printf("[drv] DMA: fake driver initialized\n");
}

static void __dma_hsave(void)
{
    // Nothing to do, this is a fake driver
    ;
}

static void __dma_hrestore(void)
{
    // Nothing to do, this is a fake driver
    ;
}

struct vhex_driver drv_dma = {
    .name       = "SDL2 DMA",
    .hsave      = (void*)&__dma_hsave,
    .hrestore   = (void*)&__dma_hrestore,
    .configure  = (void*)&__dma_configure,
    .state_size = 4,
    .flags = {
        .DMA      = 1,
        .SHARED   = 0,
        .UNUSED   = 0,
    },
    .module_data = &(struct dma_drv_interface){
        .dma_memcpy   = &sdl_dma_memcpy,
        .dma_memset   = &sdl_dma_memset,
        .dma_wait     = &sdl_dma_wait,
        .dma_spinwait = &sdl_dma_spinwait
    }
};
VHEX_DECLARE_DRIVER(05, drv_dma);
