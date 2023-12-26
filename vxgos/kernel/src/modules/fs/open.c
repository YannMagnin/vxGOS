#include <vhex/fs.h>

#include <errno.h>
#include <stdarg.h>

/* open() : try to open file */
int open(const char *pathname, int flags, ...)
{
    (void)pathname;
    (void)flags;
    return -1;
#if 0
    va_list ap;
    void *data;
    int mode;

    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);

    if (fs_info.fs.primitives.open == NULL) {
        errno = ENOMEDIUM;
        return -1;
    }

    errno = fs_table_reserve(&data);
    if (errno != 0)
        return -1;

    return fs_info.fs.primitives.open(
        data,
        pathname,
        flags,
        mode
    );
#endif
}
