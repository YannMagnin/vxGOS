#ifndef VHEX_MODULES_DISPLAY_SURFACE_H
#define VHEX_MODULES_DISPLAY_SURFACE_H

#include "vhex/defs/types.h"

//---
// Public
//---

/* dsurface - Describe the surface */
struct dsurface {
    void *vram;
    size_t width;
    size_t height;
    int x1;
    int y1;
    int x2;
    int y2;
    int number;
};

#endif /* VHEX_MODULES_DISPLAY_SURFACE_H */
