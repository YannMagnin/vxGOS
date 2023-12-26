#ifndef __VHEX_DEVICE__
# define __VHEX_DEVICE__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>

struct vhex_device
{
    /* device name */
    const char * const name;

    /* device flags */
    byte_union(flags,
        uint8_t         :1;
        uint8_t         :1;
        uint8_t         :1;
        uint8_t         :1;

        uint8_t         :1;
        uint8_t         :1;
        uint8_t         :1;
        uint8_t FS      :1;
    );

    /* device primitives */
    struct {
        int (*init)(void);
        int (*open)(void **data, const char *pathname, int flags, int mode);
        ssize_t (*write)(void *data, const void *buf, size_t size);
        ssize_t (*read)(void *data, void *buf, size_t size);
        off_t (*lseek)(void *data, off_t offset, int whence);
        int (*close)(void *data);
        int (*quit)(void);
    } primitives;
};

/* VHEX_DECLARE_DEVICE(): Declare a device for the kernel

   Use this macro to declare a device by passing it the name of a
   vhex_device structure. This macro moves the structure to the
   `.vhex.device` sections, which are automatically traversed at startup. */
#define VHEX_DECLARE_DEVICE(name) \
    VSECTION(".vhex.device") extern struct vhex_device name;


//---
// Internal driver control
//
// The following data is exposed for introspection and debugging purposes; it
// is not part of the vhex API. There is *no stability guarantee* that the
// following types and functions will remain unchanged in future minor and
// patch versions.
//---

/* Number of drivers in the (vhex_drivers) array */
#define vhex_device_count() \
    ((struct vhex_device *)&vhex_devices_end \
        - (struct vhex_device *)&vhex_devices_start)

/* driver table */
#define vhex_device_table() \
    ((struct vhex_device *)&vhex_devices_start)

/* provided by the linker script */
extern uintptr_t vhex_devices_start;
extern uintptr_t vhex_devices_end;

#endif /* __VHEX_DEVICE__ */
