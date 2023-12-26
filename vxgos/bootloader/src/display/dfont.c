#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dfont_get() : get font information */
int dfont_get(struct font **font)
{
    return _bios_dfont_get(font);
}
