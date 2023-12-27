#include <stdint.h>
#include <stddef.h>

//---
// Public
//---

/* dinfo() : get display information */
void dinfo(size_t *width, size_t *height)
{
    if (width != NULL)
        *width = 396;
    if (height != NULL)
        *height = 224;
}
