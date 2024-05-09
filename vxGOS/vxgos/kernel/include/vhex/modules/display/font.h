#ifndef VHEX_MODULES_DISPLAY_FONT_H
#define VHEX_MODULES_DISPLAY_FONT_H 1

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "vhex/defs/types.h"
#include "vhex/defs/attributes.h"

//---
// Public
//---


// types


/* vxfont: font data encoded for dfont*() functions */
struct vxfont
{
    /* font name (NUL-terminated), NULL if no title */
    char const *name;

    /* font shape flags */
    byte_union(shape,
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t                 :1;
        uint8_t                 :3;
        uint8_t proportional    :1;
    );

    /* character spacing (usually 1) */
    uint8_t char_spacing;

    /* glyph information */
    struct {
        uint8_t height;
        uint8_t line_height;
        uint32_t *data;
        uint32_t count;
        struct {
            uint8_t width;
            uint16_t char_block_size;
        } mono;
        struct __workaround {
            uint8_t width;
            uint16_t index;
            uint8_t shift;
        } *prop;
    } glyph;
};


// drawing API


/* Alignment settings for dtext_opt() and dprint_opt(). Combining a vertical
 * and a horizontal alignment option specifies where a given point (x,y)
 * should be relative to the rendered string. */
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
extern int dtext_opt(
    int x,
    int y,
    int fg,
    int bg,
    int align,
    char const * const str,
    int size
);

/* dtext() : display raw text */
extern int dtext(int x, int y, int fg, char const * const text);

/* dprint_opt(): display a formated string */
extern int dprint_opt(
    int x,
    int y,
    int fg,
    int bg,
    int align,
    char const * const str,
    ...
);

/* dprint() : display formated text */
extern int dprint(int x, int y, int fg, char const * const text, ...);

/* dvprint_opt(): Display a string of text */
extern int dvprint_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const *text,
    va_list ap
);

/* dvprint() : display formated text (vargs version) */
extern int dvprint(
    int x,
    int y,
    int fg,
    char const * const text,
    va_list ap
);

/* dchar() : render one character */
extern void dchar(uint32_t code_point, int x, int y, int fg, int bg);


// non-drawing functions


/* dfont_get() : get the current system font */
extern int dfont_get(struct vxfont **font);

/* dfont_glyph_index(): Obtain the glyph index of a Unicode code point */
extern int dfont_glyph_index(struct vxfont const *font, uint32_t code_point);

/* dfont_glyph_geometry() : get glyph geometry */
extern int dfont_glyph_geometry(
    uint32_t code_point,
    struct vxfont *font,
    size_t *width,
    size_t *height
);

/* dfont_utf8_next(): Read the next UTF-8 code point of a string */
extern uint32_t dfont_utf8_next(uint8_t const **str_pointer);

/* dfont_text_geometry() : get the rendered text geometry */
extern int dfont_text_geometry(
    void const * const text,
    int *str_len,
    struct vxfont *font,
    size_t *width,
    size_t *height
);

#endif /* VHEX_MODULES_DISPLAY_FONT_H */
