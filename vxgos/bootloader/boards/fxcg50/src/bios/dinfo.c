#include "bootloader/bios.h"

//---
// Public
//---

/* _bios_dinfo() : get display information */
void _bios_dinfo(size_t *width, size_t *height)
{
    if (width != NULL)
        *width = 396;
    if (height != NULL)
        *height = 224;
}
