#ifndef __VHEX_MODULE__
# define __VHEX_MODULE__

#include <vhex/defs/types.h>

/* kernel module */
struct vhex_module
{
    char const *name;
    void (*init)(void);
    void (*quit)(void);
};

/* VHEX_DECLARE_MODULE() : Declare a kernel module */
#define VHEX_DECLARE_MODULE(level, name) \
    VSECTION(".vhex.modules." #level) extern struct vhex_module name;


//---
// Internal module control
//
// The following data is exposed for introspection and debugging
// purposes; it is not part of the vhex API. There is *no stability
// guarantee* that the following types and functions will remain
// unchanged in future minor and patch versions.
//---

/* Number of modules in the (vhex_modules) array */
#define vhex_module_count() \
    ((struct vhex_module *)&vhex_modules_end - \
     (struct vhex_module *)&vhex_modules_start)

/* driver table */
#define vhex_module_table() \
    ((struct vhex_module *)&vhex_modules_start)

/* provided by the linker script */
extern uintptr_t vhex_modules_start;
extern uintptr_t vhex_modules_end;

#endif /* __VHEX_MODULE__ */
