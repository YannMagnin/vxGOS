//---
// drivers:screen:r61524:driver - driver declaration
//---

#include "vhex/drivers/screen/r61524.h"
#include "vhex/modules/display/interface.h"
#include "vhex/drivers/driver.h"

//---
// Public
//---

/* drv_r61524 : r61524 driver object */
struct vxdriver drv_r61524 = {
    .name       = "R61524",
    .hsave      = (void*)&r61524_hw_hsave,
    .hrestore   = (void*)&r61524_hw_hrestore,
    .configure  = (void*)&r61524_hw_configure,
    .state_size = sizeof(struct r61524_drv_ctx),
    .flags = {
        .DISPLAY = 1,
        .SHARED  = 0,
        .UNUSED  = 0,
    },
    .internal_data = &(struct dstack_drv_interface){
        .display    = {
            .frame_start        = &r61524_frame_start,
            .frame_frag_next    = &r61524_frame_frag_next,
            .frame_frag_send    = &r61524_frame_frag_send,
            .frame_end          = &r61524_frame_end,
        },
        .bios       = {
            .dclear             = &r61524_hw_dclear,
            .dpixel             = &r61524_hw_dpixel,
            .dscroll            = &r61524_hw_dscroll,
        },
        .screen     = {
            .width              = R61524_SCREEN_WIDTH,
            .height             = R61524_SCREEN_HEIGHT,
        },
    }
};
VHEX_DECLARE_DRIVER(16, drv_r61524);
