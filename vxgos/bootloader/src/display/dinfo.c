#include <stdint.h>
#include <stddef.h>

#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dinfo() : get display information */
void dinfo(size_t *width, size_t *height)
{
    _bios_dinfo(width, height);
}
