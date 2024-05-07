//---
// modules:display:font:text    - text/string handling
//---

#include "vhex/modules/display/font.h"

//---
// Public
//---

/* dfont_text_geometry() : get the rendered text geometry
 *
 * @note
 * - exept for the string, all other arguments can be NULL
 * - if the font is NULL, then the current system font will be used */
int dfont_text_geometry(
    void const * const text,
    int *str_len,
    struct vxfont *font,
    size_t *width,
    size_t *height
) {
    uint8_t const *str0;
    uint8_t const *str1;
    uint32_t code_point;
    size_t length;
    size_t max_x;
    size_t max_y;
    size_t x;
    size_t glyph_width;
    size_t glyph_height;
    int limit;

    if (font == NULL) {
        if (dfont_get(&font) != 0)
            return -1;
    }
    limit = -1;
    if (str_len != NULL && *str_len >= 0)
        limit = *str_len;

    str0    = text;
    str1    = text;
    length  = 0;
    max_x   = 0;
    max_y   = 0;
    x       = 0;

    while (true)
    {
        code_point = dfont_utf8_next(&str0);
        if (code_point == 0 || (limit > 0 && str0 - str1 >= limit))
            break;
        if (
            dfont_glyph_geometry(
                code_point,
                font,
                &glyph_width,
                &glyph_height
            ) != 0
        ) {
            break;
        }
        length += 1;
        if (code_point == '\n') {
            max_y += glyph_height;
            x = 0;
            continue;
        }
        x += glyph_width + font->char_spacing;
        if (x > max_x)
            max_x = x;
    }

    if (width != NULL) *width = max_x - font->char_spacing;
    if (height != NULL) *height = max_y;
    if (str_len != NULL) *str_len = length;
    return (0);
}
