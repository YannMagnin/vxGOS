#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dupdate() : update VRAM to screen */
void dupdate(void)
{
    _bios_dupdate();
}
