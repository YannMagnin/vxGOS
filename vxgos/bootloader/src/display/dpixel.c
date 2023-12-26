#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dpixel() : draw pixel */
void dpixel(int x, int y, int color)
{
    _bios_dpixel(x, y, color);
}
