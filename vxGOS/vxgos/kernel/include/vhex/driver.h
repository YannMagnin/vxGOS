#ifndef __VHEX_DRIVER__
# define __VHEX_DRIVER__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

/* Device drivers */
struct vhex_driver
{
    /* Driver name */
    char const *name;

    // "Hypervisor" calls

    /* Determine whether the device is powered. If NULL, the device is
       assumed to be permanently powered. */
    bool (*hpowered)(void);
    /* Power on the device; this should allow register access to save the
       peripheral state, with minimal state changes. Cannot be NULL if
       hpowered() can return false. */
    void (*hpoweron)(void);
    /* Power off the device; Cannot be NULL if hpowered() can return
       false. */
    void (*hpoweroff)(void);

    /* Save the hardware state; the (state) pointer points to a 4-aligned
       region of (state_size) bytes. */
    void (*hsave)(void *state);
    /* Restore a hardware state previously saved by hsave(). */
    void (*hrestore)(void const *state);

    // Standard calls

    /* Initialize the hardware for the driver to work in vhex. Usually
       installs interrupt handlers and configures registers. Cannot be
       NULL */
    void (*configure)(void *state);

    // Internal information

    /* Size of the peripheral's hardware state (assumed 4-aligned) */
    uint16_t state_size;

    /* default flags for the driver */
    byte_union(flags,
        uint8_t RTC         :1;
        uint8_t KEYBOARD    :1;
        uint8_t TIMER       :1;
        uint8_t DISPLAY     :1;

        uint8_t DMA         :1;
        uint8_t USB         :1;
        uint8_t SHARED      :1;
        uint8_t UNUSED      :1;
    );

    /* module related-internal data */
    void *module_data;
};


/* VHEX_DECLARE_DRIVER(): Declare a driver to the kernel

   Use this macro to declare a driver by passing it the name of a
   vhex_world_core structure. This macro moves the structure to the
   .vhex.drivers.* sections, which are automatically traversed at startup.

   The level argument represents the priority level: lower numbers mean that
   drivers will be loaded sooner. This numbering allows a primitive form of
   dependency for drivers. You need to specify a level which is strictly
   higher than the level of all the drivers you depend on.

   The level number *MUST HAVE EXACTLY 2 DIGITS*, as it is used as a string in
   the section name and the linker then sorts by name. If your driver has a
   level lower than 10, you must add a leading 0. */
#define VHEX_DECLARE_DRIVER(level, name) \
    VSECTION(".vhex.drivers." #level) extern struct vhex_driver name;




//---
// Internal driver control
//
// The following data is exposed for introspection and debugging purposes; it
// is not part of the vhex API. There is *no stability guarantee* that the
// following types and functions will remain unchanged in future minor and
// patch versions.
//---

/* Number of drivers in the (vhex_drivers) array */
#define vhex_driver_count() \
    ((struct vhex_driver *)&vhex_drivers_end - \
     (struct vhex_driver *)&vhex_drivers_start)

/* driver table */
#define vhex_driver_table() \
    ((struct vhex_driver *)&vhex_drivers_start)

/* provided by the linker script */
extern uintptr_t vhex_drivers_start;
extern uintptr_t vhex_drivers_end;

#endif /* __VHEX_DRIVER__ */
