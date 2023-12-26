#include <vhex/display/font.h>

/* dfont_utf8_next(): Read the next UTF-8 code point of a string */
uint32_t dfont_utf8_next(uint8_t const **str_pointer)
{
    uint8_t const *str = *str_pointer;
    uint8_t lead = *str++;

    /* Skip non-leaders which are invalid as starting bytes */
    while((lead >= 0x80 && lead <= 0xbf) ||
        lead == 0xc0 || lead == 0xc1 || lead == 0xfe || lead == 0xff) {
        lead = *str++;
    }

    /* This base case will handle the NUL terminator */
    if(lead <= 0x7f) {
        *str_pointer = str;
        return lead;
    }

    uint8_t n2 = (*str++ & 0x3f);
    if(lead <= 0xdf) {
        *str_pointer = str;
        return ((lead & 0x1f) << 6) | n2;
    }

    uint8_t n3 = (*str++ & 0x3f);
    if(lead <= 0xef) {
        *str_pointer = str;
        return ((lead & 0x0f) << 12) | (n2 << 6) | n3;
    }

    uint8_t n4 = (*str++ & 0x3f);
    if(lead <= 0xf7) {
        *str_pointer = str;
        return ((lead & 0x07) << 18) | (n2 << 12) | (n3 << 6) | n4;
    }

    /* It the string is too invalid, force a space and try to continue */
    *str_pointer = str;
    return 0x20;
}

#if 0
/* dfont_glyph_index(): Obtain the glyph index of a Unicode code point */
int dfont_glyph_index(font_t const *f, uint32_t code_point)
{
    int glyph_start = 0;

    for(int i = 0; i < f->block_count; i++) {
        int diff = code_point - f->blocks[i].start;
        if(diff >= 0 && diff < f->blocks[i].length) {
            return glyph_start + diff;
        }

        glyph_start += f->blocks[i].length;
    }

    return -1;
}
#endif

/* dfont_glyph_index(): Obtain the glyph index of a Unicode code point */
//TODO: suport unicode block
int dfont_glyph_index(font_t const *f, uint32_t code_point)
{
    code_point -= ' ';
    if ((int)code_point < 0 || code_point >= f->glyph.count)
        return (-1);
    return code_point;
}

/* dfont_get() : get the current font */
font_t *dfont_get(void)
{
    extern font_t font8x9;
    return &font8x9;
}

/* dfont_text_geometry() : get the rendered text geometry with a given font */
int dfont_text_geometry(
    font_t *font,
    char const * const str_char,
    int *size,
    size_t *w,
    size_t *h
) {
    uint8_t const *str0 = (void *)str_char;
    uint8_t const *str1 = (void *)str_char;
    uint32_t code_point;
    size_t length = 0;
    size_t mx = 0;
    size_t x = 0;
    size_t y = 0;
    size_t glyph_width;
    int glyph_idx;
    int limit;

    /* handle special behaviour */
    if (font == NULL)
        font = dfont_get();
    limit = -1;
    if (size != NULL && *size >= 0)
        limit = *size;

    /* generate geometry information */
    while (1) {
        code_point = dfont_utf8_next(&str0);
        if (code_point == 0 || (limit > 0 && str0 - str1 >= limit))
            break;

        /* line discipline */
        length++;
        if (code_point == '\n') {
            y += font->glyph.height;
            x = 0;
            continue;
        }

        glyph_width = font->glyph.mono.width;
        if (font->shape.prop == 1) {
            glyph_idx = dfont_glyph_index(font, code_point);
            if (glyph_idx < 0) {
                if (w != NULL)    *w = 0;
                if (h != NULL)    *h = 0;
                if (size != NULL) *size = length;
                return (-1);
            }
            glyph_width = font->glyph.prop[glyph_idx].width;
        }

        x += glyph_width + font->char_spacing;
        if (x > mx) mx = x;
    }

    /* set geometry information */
    if (w != NULL)    *w = mx - font->char_spacing;
    if (h != NULL)    *h = y + font->glyph.height;
    if (size != NULL) *size = length;
    return (0);
}
