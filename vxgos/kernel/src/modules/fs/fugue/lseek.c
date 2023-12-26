#include <vhex/fs/fugue.h>

off_t fugue_lseek(void *data, off_t offset, int whence)
{
    (void)data;
    (void)offset;
    (void)whence;
    return -1;
}
