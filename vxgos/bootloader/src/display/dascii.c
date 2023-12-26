#include "bootloader/display.h"

//---
// Public
//---

/* dascii() : draw char using provided font */
void dascii(int x, int y, int color, char n, struct font *font)
{
    uint32_t glyph_bitmap;
    uint32_t glyph_shift;
    int glyph_width;
    int glyph_size;
    int glyph_idx;
    int counter;
    int sx;

    if (n <= ' ')
        return;

    if (font == NULL && dfont_get(&font) != 0)
        return;

    /* pre-calculate bitmap position information
     * @note
     * - font data SHOULD be 32-aligned
     * - one data entry hold 32 pixels */
    glyph_width = font->width;
    glyph_size  = font->char_block_size;
    glyph_shift = glyph_size * (n - ' ');
    glyph_idx   = glyph_shift >> 5;
    glyph_shift = glyph_shift & 0x1f;

    /* simple drawing algo */
    sx = x;
    counter = 0;
    glyph_shift = 0x80000000 >> glyph_shift;
    glyph_bitmap = font->data[glyph_idx];
    for (int i = 0; i < glyph_size; ++i)
    {
        if (glyph_bitmap & glyph_shift) {
            dpixel(x, y, color);
        }

        /* update font bitmap index / shift */
        glyph_shift >>= 1;
        if (glyph_shift == 0x00000000) {
            glyph_shift = 0x80000000;
            glyph_bitmap = font->data[++glyph_idx];
        }

        /* update bitmap position */
        x += 1;
        counter += 1;
        if (counter >= glyph_width) {
            counter = 0;
            x = sx;
            y += 1;
        }
    }
}
