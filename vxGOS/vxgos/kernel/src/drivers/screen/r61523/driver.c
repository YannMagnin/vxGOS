//---
// drivers:screen:r61523:driver - driver declaration
//---

#include "vhex/drivers/screen/r61523.h"
#include "vhex/modules/display/interface.h"
#include "vhex/drivers/driver.h"

//---
// Public
//---

/* drv_r61523 : declare r61523 driver */
struct vxdriver drv_r61523 = {
    .name       = "R61523",
    .hsave      = NULL, //(void*)&r61523_hw_hsave,
    .hrestore   = NULL, //(void*)&r61523_hw_hrestore,
    .configure  = NULL, //(void*)&r61523_hw_configure,
    .state_size = sizeof(struct r61523_drv_ctx),
    .flags = {
        .DISPLAY = 1,
        .SHARED  = 0,
        .UNUSED  = 0,
    },
    .internal_data = &(struct dstack_drv_interface){
        .display = {
            .frame_start        = &r61523_frame_start,
            .frame_frag_next    = &r61523_frame_frag_next,
            .frame_frag_send    = &r61523_frame_frag_send,
            .frame_end          = &r61523_frame_end,
        },
        .bios   = {
            .dclear             = &r61523_hw_dclear,
            .dpixel             = &r61523_hw_dpixel,
            .dscroll            = &r61523_hw_dscroll,
        },
        .screen = {
            .width              = 320,
            .height             = 528,
        },
    },
};
VHEX_DECLARE_DRIVER(16, drv_r61523);
