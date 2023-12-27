#include "bootloader/bios.h"
#include "bootloader/display.h"
#include "raspi3b/framebuffer.h"

//---
// Public
//---

/* _bios_dpixel() : dpixel wrapper */
void _bios_dpixel(int x, int y, int color)
{
    struct framebuffer *framebuffer;
    uint32_t *vram;

    if (framebuffer_get(&framebuffer) != 0)
        return;
    if ((unsigned)x >= framebuffer->phys.width)
        return;
    if ((unsigned)y >= framebuffer->phys.height)
        return;

    vram = (void *)framebuffer->vram.cpu;
    switch (color)
    {
        case C_WHITE:
            vram[(y * framebuffer->phys.width) + x] = 0xffffffff;
            break;
        case C_BLACK:
            vram[(y * framebuffer->phys.width) + x] = 0x00000000;
            break;
    }
}
