//---
// drivers:screen:t6k11:driver  - driver declaration
//---

#include "vhex/drivers/screen/t6k11.h"
#include "vhex/modules/display/interface.h"
#include "vhex/drivers/driver.h"

//---
// Public
//---

/* drv_t6k11 : r6k11 driver object */
struct vxdriver drv_t6k11 = {
    .name       = "T6K11",
    .hsave      = NULL,
    .hrestore   = NULL,
    .configure  = (void*)&t6k11_hw_configure,
    .state_size = sizeof(struct t6k11_drv_ctx),
    .flags = {
        .DISPLAY = 1,
        .SHARED  = 0,
        .UNUSED  = 0,
    },
    .internal_data = &(struct dstack_drv_interface){
        .display    = {
            .frame_start        = &t6k11_frame_start,
            .frame_frag_next    = &t6k11_frame_frag_next,
            .frame_frag_send    = &t6k11_frame_frag_send,
            .frame_end          = &t6k11_frame_end,
        },
        .bios       = {
            .dclear             = &t6k11_hw_dclear,
            .dpixel             = &t6k11_hw_dpixel,
            .dscroll            = &t6k11_hw_dscroll,
        },
        .screen     = {
            .width              = T6K11_SCREEN_WIDTH,
            .height             = T6K11_SCREEN_HEIGHT,
        },
    }
};
VHEX_DECLARE_DRIVER(16, drv_t6k11);
