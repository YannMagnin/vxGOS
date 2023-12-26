#include "bootloader/bios.h"
#include "raspi3b/framebuffer.h"

//---
// Public
//---

/* _bios_dinfo() : get display information */
void _bios_dinfo(size_t *width, size_t *height)
{
    struct framebuffer *framebuffer;

    if (framebuffer_get(&framebuffer) != 0)
        return;
    if (width != NULL)
        *width = framebuffer->phys.width;
    if (height != NULL)
        *height = framebuffer->phys.height;
}
