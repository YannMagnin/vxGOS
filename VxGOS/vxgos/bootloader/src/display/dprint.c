#include <stdarg.h>
#include <alloca.h>
#include <stdio.h>

#include "bootloader/display.h"
#include "bootloader/bios.h"

//---
// Public
//---

/* dprint() : printf-like dtext() */
void dprint(int x, int y, int color, char const *format, ...)
{
    char *buffer;
    va_list ap;

    buffer = alloca(128);
    if (buffer == NULL)
        return;

    va_start(ap, format);
    vsnprintf(buffer, 128, format, ap);
    va_end(ap);

    dtext(x, y, color, buffer);
}
