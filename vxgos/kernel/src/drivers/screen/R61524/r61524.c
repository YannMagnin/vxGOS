#include <vhex/defs/types.h>
#include <vhex/driver.h>
#include <vhex/driver/screen/r61524.h>
#include <vhex/driver/mpu/sh/sh7305/dma.h>
#include <vhex/display/interface.h>

//---
// R61524 driver API
//---

/* defined by the linker script */
extern uint32_t vhex_vram0;
extern uint32_t vhex_vram1;

static uint32_t *vhex_vram = &vhex_vram0;
static bool r61524_atomic_transfert = false;
static dma_id_t dma = -1;

/* r61524_frame_start() - prepar the screen and reset surfaces */
VALIGNED(4) int r61524_frame_start(dsurface_t *surface)
{
    // NOTE IF YOU WHANT TO ENABLE FRAGMENT AGAIN
    // You need to force the windows configuration (in
    // r61524_frame_frag_send) !!!

    /* initialize surface information */
//  surface->vram = (void*)0xe5017000;
    surface->vram = (void*)vhex_vram;
    surface->width = 396;
    surface->height = 224;
//  surface->height = 10;
    surface->x1 = 0;
    surface->y1 = 0;
    surface->x2 = 395;
    surface->y2 = 223;
//  surface->y2 = 9;
    return (0);
}

VALIGNED(4) int r61524_frame_frag_next(dsurface_t *surface)
{
    (void)surface;
    return -1;
#if 0
    surface->y1 += 10;
    if (surface->y1 >= 224) {
        surface->y1 -= 10;
        return (-1);
    }

    if (surface->y1 >= 220)
        surface->height = 4;
    surface->y2 += surface->height;
    return (0);
#endif
}


VALIGNED(4) VWEAK int r61524_frame_frag_send(dsurface_t *surface)
{
    /* wait last DMA transfert */
    if (dma >= 0)
        sh7305_dma_wait(dma);

#if 0
    /* Set the windows size */
    r61524_select(horizontal_ram_start);
    r61524_write(0);
    r61524_select(horizontal_ram_end);
    r61524_write(395);
    r61524_select(vertical_ram_start);
    r61524_write(0);
    r61524_select(vertical_ram_end);
    r61524_write(223);
#endif

    /* Set the RAM position */
    r61524_select(ram_address_horizontal);
    r61524_write(0);
    r61524_select(ram_address_vertical);
    r61524_write(0);

    /* Bind address 0xb4000000 to the data write command */
    r61524_select(write_data);

    if (r61524_atomic_transfert) {
        uint16_t *vram = surface->vram;
        for (int i = 0; i < 396*224; ++i) {
            r61524_write(vram[i]);
        }
    } else {
        dma = sh7305_dma_transfert_async(
            (uintptr_t)surface->vram, DMA_ADDR_INC,
            (uintptr_t)0xb4000000,    DMA_ADDR_FIXED,
            DMA_BLOCK_SIZE_32B,
            5544
        );
        vhex_vram = (vhex_vram == &vhex_vram0) ? &vhex_vram1 : &vhex_vram0;
        surface->vram = vhex_vram;
    }

#if 0
    uint16_t * restrict yram = surface->vram;
    int size = (surface->y1 == 220) ? 1584 : 3960;

    for (int i = 0; i < size; ++i) {
        r61524_write(yram[i]);
    }
#endif
    return (0);
}

VALIGNED(4) int r61524_frame_end(dsurface_t *surface)
{
    (void)surface;
    return (0);
}

//---
// Driver definition
//---

/* R61524 display (graphics RAM range) */
struct r61524_ctx {
    uint16_t HSA;
    uint16_t HEA;
    uint16_t VSA;
    uint16_t VEA;
};

static void __r61524_configure(struct r61524_ctx *s)
{
    /* disable double-buffering display */
    r61524_atomic_transfert = true;

    /* screen information */
    s->HSA = 0;
    s->HEA = 395;
    s->VSA = 0;
    s->VEA = 223;
}

static void __r61524_hsave(struct r61524_ctx *s)
{
    r61524_select(horizontal_ram_start);
    s->HSA = r61524_read();
    r61524_select(horizontal_ram_end);
    s->HEA = r61524_read();

    r61524_select(vertical_ram_start);
    s->VSA = r61524_read();
    r61524_select(vertical_ram_end);
    s->VEA = r61524_read();
}

static void __r61524_hrestore(struct r61524_ctx const *s)
{
    r61524_select(horizontal_ram_start);
    r61524_write(s->HSA);
    r61524_select(horizontal_ram_end);
    r61524_write(s->HEA);

    r61524_select(vertical_ram_start);
    r61524_write(s->VSA);
    r61524_select(vertical_ram_end);
    r61524_write(s->VEA);
}


struct vhex_driver drv_r61524 = {
    .name       = "R61524",
    .hsave      = (void*)&__r61524_hsave,
    .hrestore   = (void*)&__r61524_hrestore,
    .configure  = (void*)&__r61524_configure,
    .state_size = sizeof(struct r61524_ctx),
    .flags = {
        .DISPLAY = 1,
        .SHARED  = 0,
        .UNUSED  = 0,
    },
    .module_data = &(struct dstack_drv_interface){
        .frame_start     = &r61524_frame_start,
        .frame_frag_next = &r61524_frame_frag_next,
        .frame_frag_send = &r61524_frame_frag_send,
        .frame_end       = &r61524_frame_end,
        .display_width   = 396,
        .display_height  = 224
    }
};
VHEX_DECLARE_DRIVER(16, drv_r61524);
