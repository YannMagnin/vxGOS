#include <stdbool.h>

#include "bootloader/display.h"

//---
// Public
//---

/* expose this symbol here */
bool enable_font = true;
bool enable_font_error = false;

/* dfont_get() : get font information */
int dfont_get(struct font **font)
{
    extern struct font font8x9;
    extern bool enable_font;
    extern bool enable_font_error;

    if (font == NULL || enable_font_error == true)
        return -1;
    *font = NULL;
    if (enable_font == true)
        *font = &font8x9;
    return 0;
}
