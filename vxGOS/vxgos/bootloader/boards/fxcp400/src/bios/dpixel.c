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
    extern uint16_t *casio_vram;

    if ((unsigned)x >= 320)
        return;
    if ((unsigned)y >= 528)
        return;
    casio_vram[(y * 320) + x] = color & 0xffff;
    return;
}
