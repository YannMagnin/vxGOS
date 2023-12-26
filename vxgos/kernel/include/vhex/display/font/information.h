#ifndef __VHEX_DISPLAY_FONT_INFORMATION__
# define __VHEX_DISPLAY_FONT_INFORMATION__

#include <vhex/display/font/types.h>

/* dfont_get() : get the current font */
extern font_t *dfont_get(void);

/* dfont_glyph_index(): Obtain the glyph index of a Unicode code point */
extern int dfont_glyph_index(font_t const *f, uint32_t code_point);

/* dfont_utf8_next(): Read the next UTF-8 code point of a string */
extern uint32_t dfont_utf8_next(uint8_t const **str_pointer);

/* dfont_text_geometry() : get the rendered text geometry with a given font */
extern int dfont_text_geometry(
    font_t *font,
    char const * const str_char,
    int *size,
    size_t *w,
    size_t *h
);

#endif /* __VHEX_DISPLAY_FONT_INFORMATION__ */
