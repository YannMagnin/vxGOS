#include <bootloader/display.h>
#include <bootloader/bios.h>

/* sgn() (without double evaluation) */
#define sgn(s) ({               \
    __auto_type _s = (s);       \
    _s < 0 ? -1 :               \
    _s > 0 ? +1 :               \
    0;                          \
})

//---
// Public
//---

/* dline() : draw line */
void dline(int x1, int y1, int x2, int y2, int color)
{
    int i;
    int cumul;
    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int sx = sgn(dx);
    int sy = sgn(dy);

    dx = (dx >= 0 ? dx : -dx);
    dy = (dy >= 0 ? dy : -dy);

    dpixel(x1, y1, color);

    if(dx >= dy)
    {
        /* Start with a non-zero cumul to even the overdue between the
           two ends of the line (for more regularity) */
        cumul = dx >> 1;
        for(i = 1; i < dx; i++)
        {
            x += sx;
            cumul += dy;
            if(cumul > dx) {
                cumul -= dx;
                y += sy;
            }
            dpixel(x, y, color);
        }
    } else {
        cumul = dy >> 1;
        for(i = 1; i < dy; i++)
        {
            y += sy;
            cumul += dx;
            if(cumul > dy) {
                cumul -= dy;
                x += sx;
            }
            dpixel(x, y, color);
        }
    }
    dpixel(x2, y2, color);
}

#undef sgn
