#ifndef __VHEX_DISPLAY_STACK__
# define __VHEX_DISPLAY_STACK__

#include <vhex/display/shader.h>
#include <vhex/display/types.h>


/* dstack_call - dstack indirect call (same as dshader) */
struct dstack_call {
    void (*routine)(dsurface_t *surface, uintptr_t *draw_args);
    uintptr_t args[12];
};
typedef struct dstack_call dstack_call_t;

/* dstack_action - dstack drawing action information */
struct dstack_action {
    struct dstack_call call;
    struct {
        dshader_call_t *table;
        int number;
        int idx;
    } shader;
    void (*quit)(uintptr_t *arg);
};

/* DSTACK_CALL - display indirect call */
#define DSTACK_CALL(fct, ...) (dstack_call_t*)&(dstack_call_t){ \
    .routine = fct,                                             \
    .args = { __VA_ARGS__ }                                     \
}


//---
// Display stack API
//---

/* dstack_init() : Initialise the draw stack (should not be involved) */
extern int dstack_init(void);

/* dstack_add_action() : add a new action in the draw stack */
extern did_t dstack_add_action(
    dstack_call_t *call,
    dshader_call_t *shaders,
    void (*quit)(uintptr_t *args)
);

/* dstack_add_shader() : add shader on particular action */
extern int dstack_add_shader(did_t did, dshader_call_t *call);

/* dstack_render(): render a frame */
extern void dstack_render(void);

/* dstack_invalidate() : Invalidate the draw stack (reset) */
extern int dstack_invalidate(void);

/* dstack_display_width() : return the display width */
extern size_t dstack_display_width(void);

/* dstack_display_height() : return the display height */
extern size_t dstack_display_height(void);

/* dstack_quit() : Uninit the draw stack */
extern int dstack_quit(void);


#endif /* __VHEX_DISPLAY_STACK__ */
