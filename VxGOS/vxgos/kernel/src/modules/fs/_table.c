#if 0
#include <vhex/fs.h>

#include <errno.h>

/* fs_table_reserve() : reserve file descriptor */
int fs_table_reserve(void **data)
{
    *data = NULL;
    for (int i = 3 ; i < VHEX_NB_FILE; ++i) {
        if (fs_info.table[i] == NULL) {
            fs_info.table[i] = (void*)0xdeadbeef;
            *data = &fs_info.table[i];
            return 0;
        }
    }
    return ENFILE;
}

/* fs_table_find() : find device-specific data */
int fs_table_find(void **data, int fd)
{
    *data = NULL;
    if (fd < 0 || fd >= VHEX_NB_FILE)
        return EBADF;
    if (fs_info.table[fd] == NULL)
        return EBADF;
    *data = fs_info.table[fd];
    return 0;
}

/* fs_table_remove() : remove file descriptor */
int fs_table_remove(int fd)
{
    if (fd < 0 || fd >= VHEX_NB_FILE)
        return EBADF;
    if (fs_info.table[fd] == NULL)
        return EBADF;
    fs_info.table[fd] = NULL;
    return 0;
}
#endif
