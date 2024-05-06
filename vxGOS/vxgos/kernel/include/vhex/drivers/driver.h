#ifndef VHEX_DRIVERS_DRIVER_H
#define VHEX_DRIVERS_DRIVER_H 1

#include "vhex/defs/attributes.h"
#include "vhex/defs/types.h"

//---
// Public
//---

/* Device drivers */
struct vxdriver
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
 *
 * Use this macro to declare a driver by passing it the name of a
 * `vxdriver` structure. This macro moves the structure to the
 * `.vhex.drivers.*` sections, which are automatically traversed at startup.
 *
 * The level argument represents the priority level: lower numbers mean that
 * drivers will be loaded sooner. This numbering allows a primitive form of
 * dependency for drivers. You need to specify a level which is strictly
 * higher than the level of all the drivers you depend on.
 *
 * The level number *MUST HAVE EXACTLY 2 DIGITS*, as it is used as a string
 * in the section name and the linker then sorts by name. If your driver has
 * a level lower than 10, you must add a leading 0 (e.g "09" or "02"). */
#define VHEX_DECLARE_DRIVER(level, name) \
    VSECTION(".vhex.drivers." #level) extern struct vxdriver name;

// driver abstraction

/* kernel_driver_next() : feth the next driver */
extern int kernel_driver_next(struct vxdriver **driver);

/* kernel_driver_init() : find a particular driver */
extern int kernel_driver_find(struct vxdriver **driver, char *name);

#endif /* __VHEX_DRIVER__ */
