#if 0
#include <vhex/device.h>
#include <vhex/fs/fugue.h>

/* declare device */

struct vhex_device dev_fugue = {
    .name = "Fugue",
    .flags = {
        .FS = 1
    },
    .primitives = {
        .init  = NULL,
        .open  = &fugue_open,
        .write = &fugue_write,
        .read  = &fugue_read,
        .lseek = &fugue_lseek,
        .close = &fugue_close,
        .quit  = NULL
    }
};
VHEX_DECLARE_DEVICE(dev_fugue);
#endif
