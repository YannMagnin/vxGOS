#include "bootloader/display.h"

//---
// Public
//---

/*dtext() : display raw text to VRAM */
void dtext(int x, int y, int color, char const *text)
{
    struct font *font;
    int saved_x;

    if (dfont_get(&font) != 0)
        return;

    saved_x = x;
    for (int i = 0 ; text[i] != '\0' ; ++i)
    {
        if (text[i] == '\n') {
            y = y + font->height;
            x = saved_x;
            continue;
        }
        dascii(x, y, color, text[i], font);
        x += font->width;
    }
}
