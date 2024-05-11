//---
// devices:kterm:_line_discipline   - internal line discipline
//---

#include "vhex/modules/display/font.h"
#include "vhex/devices/kterm.h"

//---
// Internals
//---

/* _kterm_vertical_update() : update vertical offset */
static void _kterm_vertical_update(struct _kterm *kterm)
{
    size_t cursor_y;

    kterm->cursor.x = 0;
    cursor_y = kterm->cursor.y + kterm->display.font->glyph.height;
    if (cursor_y < kterm->display.driver.screen.height) {
        kterm->cursor.y = cursor_y;
        return;
    }
    kterm->display.driver.bios.dscroll(
        kterm->display.font->glyph.height,
        0x0000
    );
}

/* _kterm_horizontal_check() : check if the current char can be drawn */
static int _kterm_horizontal_check(struct _kterm *kterm)
{
    int glyph_index;
    size_t cursor_x;

    glyph_index = kterm->display.next_glyph_index;
    kterm->display.next_glyph_width = kterm->display.font->glyph.mono.width;
    if (kterm->display.font->shape.proportional) {
        kterm->display.next_glyph_width = \
            kterm->display.font->glyph.prop[glyph_index].width;
    }
    cursor_x = kterm->cursor.x + kterm->display.next_glyph_width;
    if (cursor_x >= kterm->display.driver.screen.width)
        return 1;
    return 0;
}

/* _kterm_char_check() : check char validity
 *
 * @note
 * - must be valid ascii
 * - must precede the space character */
static int _kterm_char_check(struct _kterm *kterm, char n)
{
    if (n < ' ')
        return -1;
    if (n >= 0x7f)
        return -2;
    kterm->display.next_glyph_index = n - ' ';
    return 0;
}


//---
// Public
//---

/* kterm_line_discipline() : ensure that the char can be drawn */
int kterm_line_discipline(struct _kterm *kterm, char n)
{
    int tabsize;
    int offset;
    int ret;

    switch (n)
    {
        case '\n':
            _kterm_vertical_update(kterm);
            kterm->cursor.x  = 0;
            return 1;
        case '\r':
            kterm->cursor.x  = 0;
            return 1;
        case '\t':
            tabsize = 4 * kterm->display.font->glyph.mono.width;
            offset  = kterm->cursor.x;
            offset  = offset - ((kterm->cursor.x / tabsize) * tabsize);
            offset  = tabsize - offset;
            offset  = kterm->cursor.x + offset;
            if ((unsigned)offset >= kterm->display.driver.screen.width) {
                offset -= kterm->display.driver.screen.width;
                _kterm_vertical_update(kterm);
            }
            kterm->cursor.x = offset;
            return 1;
        default:
            if (_kterm_char_check(kterm, n) != 0)
                return -1;
            ret = _kterm_horizontal_check(kterm);
            if (ret < 0)
                return -2;
            if (ret > 0)
                _kterm_vertical_update(kterm);
            return 0;
    }
}
