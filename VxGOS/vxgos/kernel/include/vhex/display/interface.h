#ifndef __VHEX_DISPLAY_INTERFACE__
# define __VHEX_DISPLAY_INTERFACE__

#include <vhex/display/types.h>

/* dstack_drv_interface - driver interface */
struct dstack_drv_interface {
    int (*frame_start)(dsurface_t *);
    int (*frame_frag_next)(dsurface_t *);
    int (*frame_frag_send)(dsurface_t *);
    int (*frame_end)(dsurface_t *);
    size_t display_width;
    size_t display_height;
};

#endif /* __VHEX_DISPLAY_INTERFACE__ */
