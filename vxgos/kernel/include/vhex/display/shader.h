#ifndef __VHEX_DISPLAY_SHADER__
# define __VHEX_DISPLAY_SHADER__

#include <vhex/display/types.h>

/* dshader - display shader definition */
struct dshader_call {
    void (*routine)(
        dsurface_t *surface,
        uintptr_t *draw_args,
        uintptr_t *shader_args
    );
    uintptr_t args[8];
};
typedef struct dshader_call dshader_call_t;

/* DSHADER - helper to create a shader object */
#define DSHADER(fct, ...) (dshader_call_t*)&(dshader_call_t){   \
    .routine = fct,                                             \
    .args = { __VA_ARGS__ }                                     \
}

#endif /* __VHEX_DISPLAY_SHADER__ */
