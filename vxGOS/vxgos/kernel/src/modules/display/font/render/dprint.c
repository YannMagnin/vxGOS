//---
// modules:display:font:render:dprint   - display printf-like buffer
//---

#include <stdarg.h>
#include <alloca.h>
#include <stdio.h>

#include "vhex/modules/display/font.h"
#include "vhex/modules/display/color.h"

//---
// Public
//---

/* dvprint_opt(): display a string of text */
int dvprint_opt(
    int x,
    int y,
    int fg,
    int bg,
    int align,
    char const * const text,
    va_list ap
) {
    char *buffer;

    buffer = alloca(1024);
    if (buffer == NULL)
        return -1;
    vsnprintf(buffer, 1024, text, ap);
    return (dtext_opt(x, y, fg, bg, align, buffer, -1));
}

/* dprint_opt(): display a string of text */
int dprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const *text, ...
) {
    va_list ap;
    int did;

    va_start(ap, text);
    did = dvprint_opt(x, y, fg, bg, align, text, ap);
    va_end(ap);
    return did;
}

/* dvprint() : display formatted text (vargs version) */
int dvprint(int x, int y, int fg, char const * const text, va_list ap)
{
    return (dvprint_opt(x, y, fg, C_NONE, DTEXT_LEFT | DTEXT_TOP, text, ap));
}

/* dprint() : display formatted string */
int dprint(int x, int y, int fg, char const * const text, ...)
{
    va_list ap;
    int did;

    va_start(ap, text);
    did = dvprint(x, y, fg, text, ap);
    va_end(ap);
    return did;
}


