#ifndef VHEX_MODULES_DISPLAY_STACK_H
#define VHEX_MODULES_DISPLAY_STACK_H 1

#include "vhex/modules/display/shader.h"

//---
// Public
//---

// types

/* dstack_call - dstack indirect call (same as dshader) */
struct dstack_call
{
    void (*routine)(struct dsurface *surface, uintptr_t *draw_args);
    uintptr_t args[12];
};

/* dstack_action - dstack drawing action information */
struct dstack_action
{
    struct dstack_call call;
    struct {
        struct dshader_call *table;
        int number;
        int idx;
    } shader;
    void (*quit)(uintptr_t *arg);
};

/* DSTACK_CALL - display indirect call */
#define DSTACK_CALL(fct, ...) (struct dstack_call*)&(struct dstack_call){   \
    .routine = fct,                                                         \
    .args = { __VA_ARGS__ }                                                 \
}


// kernel-level API


/* dstack_init() : initialise the draw stack (should not be involved) */
extern int dstack_init(void);

/* dstack_quit() : Uninit the draw stack */
extern int dstack_quit(void);


// user-level API


/* dstack_add_action() : add a new action in the draw stack */
extern int dstack_add_action(
    struct dstack_call *call,
    struct dshader_call *shaders,
    void (*quit)(uintptr_t *args)
);

/* dstack_add_shader() : add shader on particular action */
extern int dstack_add_shader(int did, struct dshader_call *call);

/* dstack_render(): render a frame */
extern void dstack_render(void);

/* dstack_invalidate() : Invalidate the draw stack (reset) */
extern int dstack_invalidate(void);


#endif /* __VHEX_DISPLAY_STACK__ */
