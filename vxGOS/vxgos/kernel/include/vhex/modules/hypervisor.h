#ifndef __VHEX_HYPERVISOR__
# define __VHEX_HYPERVISOR__

#include <vhex/defs/attributes.h>
#include <vhex/driver.h>

/* real Vhex world with driver bitmap and meta information for the hypervisor */
struct vhex_world {
    /* world driver information */
    struct {
        struct vhex_driver *context;
        byte_union(flags,
            uint8_t             :1;
            uint8_t             :1;
            uint8_t             :1;
            uint8_t             :1;

            uint8_t             :1;
            uint8_t SHARED      :1;
            uint8_t INIT        :1;
            uint8_t UNUSED      :1;
        );
    } *driver;

    /* internal information used by the hypervisor */
    char const * name;
    byte_union(status,
        uint8_t LOCKED          :1;
        uint8_t ACTIVE          :1;
        uint8_t                 :1;
        uint8_t                 :1;

        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t INIT            :1;
        uint8_t USED            :1;
    );
};

/* hyp_word_t : hypervisor world ID */
typedef int hyp_world_t;

//---
// User-level API
//---

/* possible returned value */
enum {
    hyp_world_already_exists    = -1,
    hyp_world_not_found         = -2,
    hyp_world_bad_id            = -3,
    hyp_world_currently_in_use  = -4,
    hyp_world_undeletable       = -5,
};

/* hypervisor_world_new() : create a new world with its name */
extern hyp_world_t hypervisor_world_new(char const * const name);

/* hypervisor_world_find() : find a world ID using its name */
extern hyp_world_t hypervisor_world_find(char const * const name);

/* hypervisor_world_get() : get a word using its ID */
extern struct vhex_world *hypervisor_world_get(hyp_world_t id);

/* hypervisor_world_switch() : perform a world switch */
extern int hypervisor_world_switch(hyp_world_t out, hyp_world_t in);

/* hypervisor_world_delete() : remove world */
extern hyp_world_t hypervisor_world_delete(hyp_world_t id);

#endif /* __VHEX_HYPERVISOR__ */
