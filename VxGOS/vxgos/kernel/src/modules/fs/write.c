#include <vhex/fs.h>

#include <errno.h>

ssize_t write(int fd, const void *buf, size_t size)
{
    (void)fd;
    (void)buf;
    (void)size;
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
    return fs_info.fs.primitives.write(data, buf, size);
#endif
}
