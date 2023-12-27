#include <vhex/fs.h>

#include <errno.h>

int close(int fd)
{
    (void)fd;
    return -1;
#if 0
    void *data;

    errno = fs_table_find(&data, fd);
    if (errno != 0)
        return -1;

    if (fd >= 0 && fd <= 2) {
        //TODO:support keyboard
        //TODO:support display
        errno = ENODEV;
        return -1;
    }

    if (fs_info.fs.primitives.write == NULL) {
        errno = EIO;
        return -1;
    }
    errno = fs_info.fs.primitives.close(data);
    if (errno != 0)
        return -1;

    return fs_table_remove(fd);
#endif
}
