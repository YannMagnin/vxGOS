#include <vhex/display/font.h>
#include <vhex/display/draw/pixel.h>
#include <vhex/display/color.h>
#include <vhex/display/stack.h>

//---
// Kernel-level render API
//---

/* dfont_char_render() : */
void dfont_char_render(
    dsurface_t *surface,
    int glyph_idx,
    uintptr_t *arg
) {
    uint32_t glyph_bitmap;
    uint32_t glyph_shift;
    int glyph_width;
    int glyph_size;
    font_t *font;
    int counter;
    int sx;
    int x;
    int y;

    x = arg[0];
    y = arg[1];


    /* generate font index / shift information */
    font = dfont_get();
    if (font->shape.prop == 1) {
        glyph_width = font->glyph.prop[glyph_idx].width;
        glyph_shift = font->glyph.prop[glyph_idx].shift;
        glyph_idx   = font->glyph.prop[glyph_idx].index;
        glyph_size  = glyph_width * font->glyph.height;
    } else {
        glyph_width = font->glyph.mono.width;
        glyph_size  = font->glyph.mono.storage_size;
        glyph_shift = glyph_size * glyph_idx;
        glyph_idx   = glyph_shift >> 8;
        glyph_shift = glyph_shift & 0xff;
    }

    /* drawing algo */
    //TODO: cache surface data (limit read/write)
    sx = x;
    counter = 0;
    glyph_shift = 0x80000000 >> glyph_shift;
    glyph_bitmap = font->glyph.data[glyph_idx];
    for (int i = 0; i < glyph_size; ++i)
    {
        if (glyph_bitmap & glyph_shift) {
            dpixel_render(surface, x, y, arg[4]);
        } else {
            dpixel_render(surface, x, y, arg[5]);
        }

        /* update font bitmap index / shift */
        glyph_shift >>= 1;
        if (glyph_shift == 0x00000000) {
            glyph_shift = 0x80000000;
            glyph_bitmap = font->glyph.data[++glyph_idx];
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

//---
// Dstack API
//---

/* dfont_char_dstack() : dstack wrapper */
void dfont_char_dstack(dsurface_t *surface, uintptr_t *arg)
{
    font_t *font = dfont_get();
    uintptr_t buff[6] = {
        arg[1], arg[2], 0, 0, arg[3], arg[4]
    };

    dfont_char_render(surface, dfont_glyph_index(font, arg[0]), buff);
}

//---
// User-level API
//---

/* dfont_char() : display one char */
void dfont_char(uint32_t n, int x, int y, int fg, int bg)
{
    dstack_add_action(
        DSTACK_CALL(
            &dfont_char_dstack,
            n,
            x, y,
            fg, bg
        ),
        NULL,
        NULL
    );
}
