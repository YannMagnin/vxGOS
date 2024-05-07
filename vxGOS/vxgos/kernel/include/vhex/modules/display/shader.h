#ifndef VHEX_MODULES_DISPLAY_SHADER_H
#define VHEX_MODULES_DISPLAY_SHADER_H 1

#include "vhex/modules/display/surface.h"

//---
// Public
//---

/* dshader - display shader definition */
struct dshader_call
{
    void (*routine)(
        struct dsurface *surface,
        uintptr_t *draw_args,
        uintptr_t *shader_args
    );
    uintptr_t args[8];
};

/* DSHADER - helper to create a shader object */
#define DSHADER(fct, ...) (struct shader_call*)&(dshader_call_t){   \
    .routine = fct,                                                 \
    .args = { __VA_ARGS__ }                                         \
}

#endif /* VHEX_MODULES_DISPLAY_SHADER_H */
