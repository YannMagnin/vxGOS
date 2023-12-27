#include <vhex/display/text.h>
#include <vhex/display/color.h>

#include <stdarg.h>
#include <stdio.h>

//---
// User-level API
//---

/* dvprint() : display formated text (vargs version) */
did_t dvprint(int x, int y, int fg, char const * const text, va_list ap)
{
    char buff[1024];

    vsnprintf(buff, 1024, text, ap);

    return (dtext_opt(x, y, fg, C_NONE, DTEXT_LEFT | DTEXT_TOP, buff, -1));
}

/* dprint() : display formated string */
did_t dprint(int x, int y, int fg, char const * const text, ...)
{
    va_list ap;
    did_t did;

    va_start(ap, text);
    did = dvprint(x, y, fg, text, ap);
    va_end(ap);

    return did;
}

/* dvprint_opt(): Display a string of text */
did_t dvprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const *text,
    va_list ap
) {
    char buff[1024];

    vsnprintf(buff, 1024, text, ap);

    return (dtext_opt(x, y, fg, bg, align, buff, -1));
}

/* dprint_opt(): Display a string of text */
did_t dprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const *text, ...
) {
    va_list ap;
    did_t did;

    va_start(ap, text);
    did = dvprint_opt(x, y, fg, bg, align, text, ap);
    va_end(ap);

    return did;
}
