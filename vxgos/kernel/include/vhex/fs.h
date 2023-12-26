#ifndef __VHEX_FS_INFORMATION__
# define __VHEX_FS_INFORMATION__

#include <vhex/defs/attributes.h>
#include <vhex/defs/types.h>
#include <vhex/device.h>

/* define the default number of file that can be oppened in the same time */
#ifndef VHEX_NB_FILE
# define VHEX_NB_FILE   8
#endif /* VHEX_NB_FILE */

/* internal file system information */
struct __fs_info {
    struct vhex_device fs;
    void *table[VHEX_NB_FILE];
};

extern struct __fs_info fs_info;

//---
// Internal API
//---

/* fs_table_reserve() : reserve file descriptor */
extern int fs_table_reserve(void **data);

/* fs_table_find() : find device-specific data */
extern int fs_table_find(void **data, int fd);

/* fs_table_remove() : remove file descriptor */
extern int fs_table_remove(int fd);

#endif /* __VHEX_FS_INFORMATION__ */
