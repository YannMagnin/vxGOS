//---
// modules:display:dstack   - display stack
//---

#include <stdlib.h>
#include <string.h>

#include "vhex/modules/display/stack.h"
#include "vhex/modules/display/shader.h"
#include "vhex/modules/display/interface.h"
#include "vhex/drivers/driver.h"

//---
// Internals
//---

/* internal display stack information */
static struct {
    struct {
        struct dstack_action *action;
        int slots;
        int idx;
    } pool;
    struct {
        int (*frame_start)(struct dsurface *);
        int (*frame_frag_next)(struct dsurface *);
        int (*frame_frag_send)(struct dsurface *);
        int (*frame_end)(struct dsurface *);
    } driver;
} dstack_info = {
    .pool = {
        .action = NULL,
        .slots = 0,
        .idx = 0
    },
    .driver = {
        .frame_start = NULL,
        .frame_frag_next = NULL,
        .frame_frag_send = NULL,
        .frame_end = NULL,
    }
};

/* dstack_action_add_shader() : add a shader in action */
static void dstack_action_add_shader(
    struct dstack_action *action,
    struct dshader_call *shader
) {
    action->shader.idx += 1;
    if (action->shader.idx >= action->shader.number)
    {
        action->shader.number += action->shader.number;
        action->shader.table = reallocarray(
            action->shader.table,
            action->shader.number,
            sizeof(struct dshader_call)
        );
    }
    memcpy(
        &action->shader.table[action->shader.idx],
        shader,
        sizeof(struct dshader_call)
    );
}

/* dstack_action_alloc() : allocate a new dstack action with shader
 *
 * @note
 * - return the "display ID" */
static int dstack_action_alloc(
    struct dstack_call *call,
    struct dshader_call *shader,
    void (*quit)(uintptr_t *arg)
) {
    struct dstack_action *action;
    int i;

    dstack_info.pool.idx += 1;
    if (dstack_info.pool.idx >= dstack_info.pool.slots)
    {
        dstack_info.pool.slots += dstack_info.pool.slots;
        dstack_info.pool.action = reallocarray(
            dstack_info.pool.action,
            dstack_info.pool.slots,
            sizeof(struct dstack_action)
        );
    }
    action = &dstack_info.pool.action[dstack_info.pool.idx];
    memcpy(&action->call, call, sizeof(struct dstack_call));
    action->shader.idx = -1;
    if (shader != NULL) {
        for (i = 0; shader[i].routine != NULL; ++i) {
            dstack_action_add_shader(action, &shader[i]);
        }
    }
    action->quit = quit;
    return dstack_info.pool.idx;
}

//---
// Public
//---

// kernel-related stuff

/* dstack_init() : initialise the draw stack
 *
 * @note
 * - should not be manually involved */
int dstack_init(void)
{
    struct vxdriver *driver;
    struct dstack_drv_interface *dinterface;

    dstack_info.pool.action = calloc(
        16,
        sizeof(struct dstack_action)
    );

    dstack_info.pool.idx = -1;
    dstack_info.pool.slots = 16;
    for (int i = 0; i < dstack_info.pool.slots; ++i)
    {
        dstack_info.pool.action[i].shader.table = calloc(
            4,
            sizeof(struct dshader_call)
        );
        dstack_info.pool.action[i].shader.number = 4;
        dstack_info.pool.action[i].shader.idx = -1;
    }

    if (kernel_driver_find(&driver, "display") != 0)
        return -1;
    dinterface = driver->internal_data;
    if (dinterface->display.frame_start == NULL)
        return -2;
    if (dinterface->display.frame_frag_next == NULL)
        return -3;
    if (dinterface->display.frame_frag_send == NULL)
        return -4;
    if (dinterface->display.frame_end == NULL)
        return -5;
    dstack_info.driver.frame_start     = dinterface->display.frame_start;
    dstack_info.driver.frame_frag_next = dinterface->display.frame_frag_next;
    dstack_info.driver.frame_frag_send = dinterface->display.frame_frag_send;
    dstack_info.driver.frame_end       = dinterface->display.frame_end;
    return 0;
}

/* dstack_quit() : uninit the display stack
 *
 * @note
 * - should not be manually involved */
int dstack_quit(void)
{
    for (int i = 0; i < dstack_info.pool.slots; ++i)
    {
        if (dstack_info.pool.action[i].shader.table != NULL)
            free(dstack_info.pool.action[i].shader.table);
        dstack_info.pool.action[i].shader.number = 0;
        dstack_info.pool.action[i].shader.idx = -1;
    }
    return 0;
}

// user-level API


/* dstack_action_get() : return the display action using its dislay ID */
struct dstack_action *dstack_action_get(int did)
{
    if (did > dstack_info.pool.idx)
        return NULL;
    return &dstack_info.pool.action[did];
}

/* dstack_add_action() : add a new action in the draw stack */
int dstack_add_action(
    struct dstack_call *call,
    struct dshader_call *shader,
    void (*quit)(uintptr_t *arg)
) {
    return dstack_action_alloc(call, shader, quit);
}

/* dstack_add_shader() : add shader on particular action */
int dstack_add_shader(int did, struct dshader_call *call)
{
    struct dstack_action *action;

    if (call == NULL)
        return -1;
    action = dstack_action_get(did);
    if (action == NULL)
        return -1;
    dstack_action_add_shader(action, call);
    return 0;
}

/* dstack_render(): render a frame */
void dstack_render(void)
{
    struct dsurface surface;
    struct dstack_action *action;

    action = dstack_info.pool.action;
    dstack_info.driver.frame_start(&surface);
    do {
        for (int i = 0; i <= dstack_info.pool.idx; ++i)
        {
            action[i].call.routine(&surface, action[i].call.args);
            for (int j = 0; j <= action[i].shader.idx; j++)
            {
                action[i].shader.table[j].routine(
                    &surface,
                    action[i].call.args,
                    action[i].shader.table[j].args
                );
            }
        }
        dstack_info.driver.frame_frag_send(&surface);
    } while (dstack_info.driver.frame_frag_next(&surface) == 0);
    dstack_info.driver.frame_end(&surface);
}

/* dstack_invalidate() : invalidate the draw stack (reset) */
int dstack_invalidate(void)
{
    struct dstack_action *action;

    action = dstack_info.pool.action;
    for (int i = 0; i <= dstack_info.pool.idx; ++i)
    {
        if (action[i].quit != NULL)
            action[i].quit(action[i].call.args);
    }
    dstack_info.pool.idx = -1;
    return 0;
}
