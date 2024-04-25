#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* _bios_dfont_get() : get font information */
int _bios_dfont_get(struct font **font)
{
    if (font == NULL)
        return -1;
    *font = NULL;
    return -2;
}
