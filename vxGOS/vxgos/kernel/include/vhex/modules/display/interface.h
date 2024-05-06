#ifndef VHEX_MODULES_DISPLAY_INTERFACE
#define VHEX_MODULES_DISPLAY_INTERFACE 1

#include "vhex/modules/display/types.h"

/* dstack_drv_interface - driver interface */
struct dstack_drv_interface {
    struct {
        int (*frame_start)(struct dsurface *);
        int (*frame_frag_next)(struct dsurface *);
        int (*frame_frag_send)(struct dsurface *);
        int (*frame_end)(struct dsurface *);
    } display;
    struct {
        int (*dpixel)(int x, int y, int color);
        int (*dclear)(int color);
        int (*dscroll)(int offset, int direction);
    } bios;
    struct {
        size_t width;
        size_t height;
    } screen;
};

#endif /* VHEX_MODULES_DISPLAY_INTERFACE */
