#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dclear() : clear VRAM with unified color */
void dclear(int color)
{
    size_t width;
    size_t height;

    dinfo(&width, &height);
    for (size_t y = 0 ; y < height ; ++y) {
        for (size_t x = 0 ; x < width ; ++x) {
            dpixel(x, y, color);
        }
    }
}
