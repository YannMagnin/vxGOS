#ifndef __VHEX_FS_FUGUE__
# define __VHEX_FS_FUGUE__

#include <vhex/defs/types.h>

extern int fugue_open(void **data, const char *pathname, int flags, int mode);
extern ssize_t fugue_write(void *data, const void *buf, size_t size);
extern ssize_t fugue_read(void *data, void *buf, size_t size);
extern off_t fugue_lseek(void *data, off_t offset, int whence);
extern int fugue_close(void *data);

#endif /* __VHEX_FS_FUGUE__ */
