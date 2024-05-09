//---
// devices:kterm:_dascii    - special ascii drawing primitive
//---

#include "vhex/devices/kterm.h"

//---
// Public
//---

/* kterm_dascii() : draw ascii char */
int kterm_dascii(struct _kterm *kterm)
{
    uint32_t glyph_bitmap;
    uint32_t glyph_shift;
    int glyph_idx;
    int glyph_width;
    int glyph_size;
    struct vxfont *font;
    int counter;
    int x;
    int y;

    x = kterm->cursor.x;
    y = kterm->cursor.y;
    font = kterm->display.font;
    glyph_idx = kterm->display.next_glyph_index;
    if (font->shape.proportional)
    {
        glyph_width = font->glyph.prop[glyph_idx].width;
        glyph_shift = font->glyph.prop[glyph_idx].shift;
        glyph_idx   = font->glyph.prop[glyph_idx].index;
        glyph_size  = glyph_width * font->glyph.height;
    } else {
        glyph_width = font->glyph.mono.width;
        glyph_size  = font->glyph.mono.char_block_size;
        glyph_shift = glyph_size * glyph_idx;
        glyph_idx   = glyph_shift >> 5;
        glyph_shift = glyph_shift & 0x1f;
    }

    counter = 0;
    glyph_shift = 0x80000000 >> glyph_shift;
    glyph_bitmap = font->glyph.data[glyph_idx];
    for (int i = 0; i < glyph_size; ++i)
    {
        if (glyph_bitmap & glyph_shift) {
            kterm->display.driver.bios.dpixel(x, y, 0xffff);
        }
        glyph_shift >>= 1;
        if (glyph_shift == 0x00000000) {
            glyph_shift  = 0x80000000;
            glyph_idx   += 1;
            glyph_bitmap = font->glyph.data[glyph_idx];
        }
        x += 1;
        counter += 1;
        if (counter >= glyph_width) {
            counter = 0;
            x = kterm->cursor.x;
            y += 1;
        }
    }

    kterm->cursor.x += kterm->display.next_glyph_width;
    return 0;
}
