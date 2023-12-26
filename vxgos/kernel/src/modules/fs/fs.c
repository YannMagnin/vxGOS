#if 0
#include <vhex/fs.h>
#include <vhex/module.h>
#include <vhex/device.h>

#include <string.h>

/* module init */

/* internal file system information */
struct __fs_info fs_info;

/* __fs_init() : initialize the fs module */
static void __fs_init(void)
{
    struct vhex_device *device = vhex_device_table();
    for (int i = 0; i < vhex_device_count(); ++i) {
        if (device[i].flags.FS) {
            memcpy(
                &fs_info.fs,
                &device[i],
                sizeof(struct vhex_device)
            );
            break;
        }
    }
    //TODO: initialise STDIN_FILENO
    //TODO: initialise STDOUT_FILENO
    //TODO: initialise STDERR_FILENO
}

/* __display_quit() : uninit the fs module */
static void __fs_quit(void)
{
    //Nothing to do
}

/* declare the fs module */

struct vhex_module mod_fs = {
    .name = "fs",
    .init = &__fs_init,
    .quit = &__fs_quit,
};
VHEX_DECLARE_MODULE(03, mod_fs);
#endif
