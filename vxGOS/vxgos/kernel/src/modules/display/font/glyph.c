//---
// modules:display:font:glyph   - font glyph handling
//---

#include "vhex/modules/display/font.h"

//---
// Public
//---

/* dfont_glyph_index(): Obtain the glyph index of a Unicode code point
 *
 * @note
 * - the font encode character drawing starting at "space" character
 * - unicode not handled for now */
int dfont_glyph_index(struct vxfont const *font, uint32_t code_point)
{
    code_point -= ' ';
    if ((int)code_point < 0 || code_point >= font->glyph.count)
        return (-1);
    return code_point;
}

/* dfont_glyph_geometry() : get glyph geometry
 *
 * @note
 * - support proportional font (each glyph have independant width)
 * - can be used to check if the character exists
 * - the provided font can be NULL, which will automatically try to fetch the
 *   system font instead */
int dfont_glyph_geometry(
    uint32_t code_point,
    struct vxfont *font,
    size_t *width,
    size_t *height
) {
    size_t glyph_width;
    int glyph_index;

    if (font == NULL) {
        if (dfont_get(&font) != 0)
            return -1;
    }
    glyph_index = dfont_glyph_index(font, code_point);
    if (glyph_index < 0)
        return -2;
    if (width != NULL)
    {
        glyph_width = font->glyph.mono.width;
        if (font->shape.proportional)
            glyph_width = font->glyph.prop[glyph_index].width;
        width[0] = glyph_width;
    }
    if (height != NULL) {
        height[0] = font->glyph.height;
    }
    return 0;
}
