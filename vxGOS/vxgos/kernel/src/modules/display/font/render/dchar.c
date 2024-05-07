//---
// modules:display:font:render:dchar    - display char
//---

#include "vhex/modules/display/font.h"
#include "vhex/modules/display/color.h"
#include "vhex/modules/display/stack.h"
#include "vhex/modules/display/surface.h"

//---
// Internals
//---

/*external symbols */
extern void dpixel_render(struct dsurface *surface, int x, int y, int color);

/* dfont_char_render() : render a character */
void dfont_char_render(
    struct dsurface *surface,
    uint32_t code_point,
    uintptr_t *arg
) {
    uint32_t glyph_bitmap;
    uint32_t glyph_shift;
    int glyph_idx;
    int glyph_width;
    int glyph_size;
    struct vxfont *font;
    int counter;
    int sx;
    int x;
    int y;

    x = arg[0];
    y = arg[1];

    /* generate font index / shift information */
    if (dfont_get(&font) != 0)
        return;
    glyph_idx = dfont_glyph_index(font, code_point);
    if (glyph_idx <= 0)
        return;
    if (font->shape.proportional == 1) {
        glyph_width = font->glyph.prop[glyph_idx].width;
        glyph_shift = font->glyph.prop[glyph_idx].shift;
        glyph_idx   = font->glyph.prop[glyph_idx].index;
        glyph_size  = glyph_width * font->glyph.height;
    } else {
        glyph_width = font->glyph.mono.width;
        glyph_size  = font->glyph.mono.char_block_size;
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

/* dfont_char_dstack() : dstack wrapper */
static void dfont_char_dstack(struct dsurface *surface, uintptr_t *arg)
{
    uintptr_t buff[6] = {
        arg[1], arg[2], 0, 0, arg[3], arg[4]
    };
    dfont_char_render(surface, arg[0], buff);
}

//---
// Public
//---

/* dchar() : display one char */
void dchar(uint32_t code_point, int x, int y, int fg, int bg)
{
    dstack_add_action(
        DSTACK_CALL(
            &dfont_char_dstack,
            code_point,
            x,
            y,
            fg,
            bg
        ),
        NULL,
        NULL
    );
}
