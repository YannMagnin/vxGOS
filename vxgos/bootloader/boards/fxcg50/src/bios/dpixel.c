#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "bootloader/bios.h"

//---
// Public
//---

/* _bios_dpixel() : dpixel wrapper */
void _bios_dpixel(int x, int y, int color)
{
    extern uint16_t vram[];

    if ((unsigned)x >= 396 || (unsigned)y >= 224)
        return;
    vram[(y * 396) + x] = color;
}
