#include <vhex/fs.h>

#include <errno.h>

off_t lseek(int fd, off_t offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
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

    if (fs_info.fs.primitives.lseek == NULL) {
        errno = EIO;
        return -1;
    }
    return fs_info.fs.primitives.lseek(data, offset, whence);
#endif
}
