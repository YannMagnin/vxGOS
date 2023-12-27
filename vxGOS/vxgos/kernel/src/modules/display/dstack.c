#include <vhex/display/stack.h>
#include <vhex/display/shader.h>
#include <vhex/display/interface.h>
#include <vhex/driver.h>

#include <stdlib.h>
#include <string.h>

static struct {
    struct {
        struct dstack_action *action;
        int slots;
        int idx;
    } pool;
    struct dstack_drv_interface driver;
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
        .display_width = 0,
        .display_height = 0
    }
};

//---
// Kernel API
//---

/* dstack_init() : Initialise the draw stack (should not be involved) */
int dstack_init(void)
{
    dstack_info.pool.action = calloc(
        16,
        sizeof(struct dstack_action)
    );
    dstack_info.pool.idx = -1;
    dstack_info.pool.slots = 16;

    for (int i = 0; i < dstack_info.pool.slots; ++i) {
        dstack_info.pool.action[i].shader.table = calloc(
            4,
            sizeof(dshader_call_t)
        );
        dstack_info.pool.action[i].shader.number = 4;
        dstack_info.pool.action[i].shader.idx = -1;
    }

    struct vhex_driver *driver = vhex_driver_table();
    for (int i = 0; i < vhex_driver_count(); ++i) {
        if (driver[i].flags.DISPLAY) {
            memcpy(
                &dstack_info.driver,
                driver[i].module_data,
                sizeof(struct dstack_drv_interface)
            );
            break;
        }
    }
    return 0;
}

/* dstack_quit() : Uninit the draw stack */
int dstack_quit(void)
{
    for (int i = 0; i < dstack_info.pool.slots; ++i) {
        free(dstack_info.pool.action[i].shader.table);
        dstack_info.pool.action[i].shader.number = 0;
        dstack_info.pool.action[i].shader.idx = -1;
    }
    return 0;
}


//---
// Internal API
//---

/* dstack_action_add_shader() : add a shader in action */
static void dstack_action_add_shader(
    struct dstack_action *action,
    dshader_call_t *shader
) {
    action->shader.idx += 1;
    if (action->shader.idx >= action->shader.number) {
        action->shader.number += action->shader.number;
        action->shader.table = reallocarray(
            action->shader.table,
            action->shader.number,
            sizeof(dshader_call_t)
        );
    }
    memcpy(
        &action->shader.table[action->shader.idx],
        shader,
        sizeof(dshader_call_t)
    );
}

/* dstack_action_alloc() : allocate a new dstack action with shader */
static did_t dstack_action_alloc(
    dstack_call_t *call,
    dshader_call_t *shader,
    void (*quit)(uintptr_t *arg)
) {
    struct dstack_action *action;
    int i;

    dstack_info.pool.idx += 1;
    if (dstack_info.pool.idx >= dstack_info.pool.slots) {
        dstack_info.pool.slots += dstack_info.pool.slots;
        dstack_info.pool.action = reallocarray(
            dstack_info.pool.action,
            dstack_info.pool.slots,
            sizeof(struct dstack_action)
        );
    }
    action = &dstack_info.pool.action[dstack_info.pool.idx];
    memcpy(&action->call, call, sizeof(dstack_call_t));
    action->shader.idx = -1;
    if (shader != NULL) {
        for (i = 0; shader[i].routine != NULL; ++i) {
            dstack_action_add_shader(action, &shader[i]);
        }
    }
    action->quit = quit;
    return dstack_info.pool.idx;
}

/* dstack_action_get() : return the display action using its dislay ID */
struct dstack_action *dstack_action_get(did_t did)
{
    if (did > dstack_info.pool.idx)
        return NULL;
    return &dstack_info.pool.action[did];
}

//---
// Public API
//---

/* dstack_add_action() : add a new action in the draw stack */
did_t dstack_add_action(
    dstack_call_t *call,
    dshader_call_t *shader,
    void (*quit)(uintptr_t *arg)
) {
    return dstack_action_alloc(call, shader, quit);
}

/* dstack_add_shader() : add shader on particular action */
int dstack_add_shader(did_t did, dshader_call_t *call)
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
    dsurface_t surface;
    struct dstack_action *action;

    action = dstack_info.pool.action;

    dstack_info.driver.frame_start(&surface);
    do {
        for (int i = 0; i <= dstack_info.pool.idx; ++i) {
            action[i].call.routine(&surface, action[i].call.args);
            for (int j = 0; j <= action[i].shader.idx; j++) {
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

/* dstack_invalidate() : Invalidate the draw stack (reset) */
int dstack_invalidate(void)
{
    struct dstack_action *action;

    action = dstack_info.pool.action;
    for (int i = 0; i <= dstack_info.pool.idx; ++i) {
        if (action[i].quit != NULL)
            action[i].quit(action[i].call.args);
    }
    dstack_info.pool.idx = -1;
    return 0;
}


/* dstack_display_width() : return the display width */
size_t dstack_display_width(void)
{
    return dstack_info.driver.display_width;
}

/* dstack_display_height() : return the display height */
size_t dstack_display_height(void)
{
    return dstack_info.driver.display_height;
}
