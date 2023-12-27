#ifndef __VHEX_DISPLAY_TEXT_RENDER__
# define __VHEX_DISPLAY_TEXT_RENDER__

#include <vhex/defs/types.h>
#include <vhex/display/types.h>

/* Alignment settings for dtext_opt() and dprint_opt(). Combining a vertical
   and a horizontal alignment option specifies where a given point (x,y) should
   be relative to the rendered string. */
enum {
    /* Horizontal settings: default in dtext() is DTEXT_LEFT */
    DTEXT_LEFT   = 0x01,
    DTEXT_CENTER = 0x02,
    DTEXT_RIGHT  = 0x04,
    /* Vertical settings: default in dtext() is DTEXT_TOP */
    DTEXT_TOP    = 0x10,
    DTEXT_MIDDLE = 0x20,
    DTEXT_BOTTOM = 0x40,
};

/* dtext_opt(): Display a string of text */
extern did_t dtext_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const * const str, int size
);

/* dtext() : display raw text */
extern did_t dtext(int x, int y, int fg, char const * const text);

/* dprint_opt(): Display a formated string */
extern did_t dprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const * const str,
    ...
);

/* dprint() : display formated text */
extern did_t dprint(int x, int y, int fg, char const * const text, ...);

#include <stdarg.h>

/* dvprint_opt(): Display a string of text */
extern did_t dvprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const *text,
    va_list ap
);

/* dvprint() : display formated text (vargs version) */
extern did_t dvprint(int x, int y, int fg, char const * const text, va_list ap);

#endif /* __VHEX_DISPLAY_TEXT_RENDER__ */
