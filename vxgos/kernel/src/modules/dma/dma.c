#include <vhex/dma/interface.h>
#include <vhex/dma.h>
#include <vhex/module.h>
#include <vhex/driver.h>

#include <string.h>

/* internal timer information */
struct {
    struct dma_drv_interface driver;
} dma_info;

/* __dma_init() : initialize the DMA module */
static void __dma_init(void)
{
    memset(&dma_info, 0x00, sizeof(dma_info));

    struct vhex_driver *driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.DMA) {
            memcpy(
                &dma_info.driver,
                driver[i].module_data,
                sizeof(struct dma_drv_interface)
            );
            break;
        }
    }
}

/* __dma_quit() : uninitialize the DMA module */
static void __dma_quit(void)
{
    ;
}

/* declare the timer module */

struct vhex_module mod_dma = {
    .name = "DMA",
    .init = &__dma_init,
    .quit = &__dma_quit,
};
VHEX_DECLARE_MODULE(04, mod_dma);
