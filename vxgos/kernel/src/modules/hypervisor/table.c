//---
// hypervisor:table - world allocator
//---
#include <vhex/hypervisor.h>

#include <string.h>
#include <stdlib.h>

/* internal world table */
static struct {
    struct {
        struct vhex_world *table;
        int number;
    } world;
} hyp = {
    .world = {
        .table = NULL,
        .number = 0
    }
};

/* hypervisor_world_new() : create a new world with its name */
hyp_world_t hypervisor_world_new(char const * const restrict name)
{
    struct vhex_driver *dtable;
    hyp_world_t id;

    /* try to find free world */
    id = -1;
    for (int i = 0; i < hyp.world.number; ++i) {
        if (hyp.world.table[i].status.USED == 0) {
            id = i;
            continue;
        }
        if (strcmp(hyp.world.table[i].name, name) == 0)
            return (hyp_world_already_exists);
    }

    /* if no world is found, update the table */
    if (id == -1) {
        id = hyp.world.number;

        hyp.world.number += hyp.world.number;
        if (hyp.world.number == 0)
            hyp.world.number = 2;

        hyp.world.table = reallocarray(
            hyp.world.table,
            hyp.world.number,
            sizeof(struct vhex_world)
        );

        for (int i = id; i < hyp.world.number; ++i) {
            memset(
                &hyp.world.table[i],
                0x00,
                sizeof(struct vhex_world)
            );
            hyp.world.table[i].status.USED = 0;
        }
    }

    /* initialize the world */
    hyp.world.table[id].name = strdup(name);
    hyp.world.table[id].status.LOCKED = 0;
    hyp.world.table[id].status.ACTIVE = 0;
    hyp.world.table[id].status.INIT   = 0;
    hyp.world.table[id].status.USED   = 1;

    if (hyp.world.table[id].driver == NULL) {
        hyp.world.table[id].driver = calloc(
            vhex_driver_count(),
            sizeof(*hyp.world.table[id].driver)
        );
        dtable = vhex_driver_table();
        for (int i = 0; i < vhex_driver_count(); ++i) {
            hyp.world.table[id].driver[i].context = malloc(
                dtable[i].state_size
            );
        }
    }

    return (id);
}


/* hypervisor_world_find() : find a world using its name */
hyp_world_t hypervisor_world_find(char const * const restrict name)
{
    if (hyp.world.table != NULL) {
        for (int i = 0; i < hyp.world.number; ++i) {
            if (strcmp(hyp.world.table[i].name, name) == 0)
                return (i);
        }
    }
    return (hyp_world_not_found);
}

/* hypervisor_world_get() : get a word using its ID */
struct vhex_world *hypervisor_world_get(hyp_world_t id)
{
    if (id < 0 || id >= hyp.world.number)
        return (NULL);
    if (hyp.world.table[id].status.USED == 0) {
        return (NULL);
    }
    return (&hyp.world.table[id]);
}


/* hypervisor_world_delete() : remove world */
hyp_world_t hypervisor_world_delete(hyp_world_t id)
{
    if (id < 0 || id >= hyp.world.number)
        return (hyp_world_bad_id);

    if (hyp.world.table[id].status.LOCKED == 1)
        return (hyp_world_undeletable);

    hyp.world.table[id].status.USED = 0;
    return 0;
}
