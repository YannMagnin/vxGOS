#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "bootloader/bios.h"
#include "bootloader/display.h"

//---
// Public
//---

/* _bios_dpixel() : dpixel wrapper */
void _bios_dpixel(int x, int y, int color)
{
    extern uint8_t vram[];
    int mask;
    int indx;

    if ((unsigned)x >= 128 || (unsigned)y >= 64)
        return;

    indx = ((y * 128) + x) / 8;
    mask = 0x80 >> (x & 7);

    switch (color) {
        case C_BLACK:
            vram[indx] |= mask;
            break;
        case C_WHITE:
            vram[indx] &= ~mask;
            break;
    }
}
