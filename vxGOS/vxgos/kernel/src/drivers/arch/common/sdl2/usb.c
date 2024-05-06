#include <vhex/usb/interface.h>
#include <vhex/driver.h>

#include <stdio.h>

static int sdl2_usb_open(void)
{
    printf("SDL2 : USB driver not implemented\n");
    return -1;
}

static int sdl2_usb_pipe_find(void)
{
    return 0;
}

/* declare driver */
struct vhex_driver drv_usb = {
    .name       = "USB",
    .hpowered   = NULL,
    .hpoweron   = NULL,
    .hpoweroff  = NULL,
    .hsave      = NULL,
    .hrestore   = NULL,
    .configure  = NULL,
    .state_size = 4,
    .flags = {
        .USB    = 1,
        .SHARED = 0,
        .UNUSED = 1,
    },
    .module_data = &(struct usb_drv_interface){
        .open      = (void*)&sdl2_usb_open,
        .pipe_find = (void*)&sdl2_usb_pipe_find
    }
};
VHEX_DECLARE_DRIVER(06, drv_usb);
