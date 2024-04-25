#include "bootloader/bios.h"

//---
// Public
//---

/* _bios_dinfo() : get display information */
void _bios_dinfo(size_t *width, size_t *height)
{
    if (width != NULL)
        *width = 320;
    if (height != NULL)
        *height = 528;
}
