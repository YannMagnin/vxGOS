#ifndef __VHEX_DISPLAY_TYPES__
# define __VHEX_DISPLAY_TYPES__

#include <vhex/defs/types.h>

/* draw ID */
typedef int did_t;

/* dsurface - Describe the surface */
struct dsurface {
    void *vram;
    size_t width;
    size_t height;
    int x1;
    int y1;
    int x2;
    int y2;
};
typedef struct dsurface dsurface_t;

#endif /* __VHEX_DISPLAY_TYPES__ */
