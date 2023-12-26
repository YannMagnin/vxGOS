#include <vhex/display/types.h>
#include <vhex/display/stack.h>
#include <vhex/display/font.h>
#include <vhex/display/text.h>
#include <vhex/display.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* internal text information/cache */
struct {
    struct {
        struct {
            char *raw;
            size_t size;
        } *text;
        int number;
        int idx;
    } pool;
} dtext_info = {
    .pool = {
        .text = NULL,
        .number = 0,
        .idx = 0
    }
};

//---
// Internal module functions
//---

VCONSTRUCTOR static void __dtext_constructor(void)
{
    dtext_info.pool.number = 32;
    dtext_info.pool.idx = -1;
    dtext_info.pool.text = calloc(
        dtext_info.pool.number,
        sizeof(*dtext_info.pool.text)
    );
    for (int i = 0; i < dtext_info.pool.number; ++i) {
        dtext_info.pool.text[i].raw = malloc(32);
        dtext_info.pool.text[i].size = 32;
    }
}

VDESTRUCTOR void __dtext_destructor(void)
{
    for (int i = 0; i < dtext_info.pool.number; ++i)
        free(dtext_info.pool.text[i].raw);
    free(dtext_info.pool.text);
}

static void __dtext_quit(void)
{
    dtext_info.pool.idx = -1;
}

//---
// Private functions
//---

/* dtext_info_register() : duplicate the string */
static char *dtext_info_register(char const * const str)
{
    size_t len;

    dtext_info.pool.idx += 1;
    if (dtext_info.pool.idx >= dtext_info.pool.number) {
        dtext_info.pool.number += dtext_info.pool.number;
        dtext_info.pool.text = reallocarray(
            dtext_info.pool.text,
            dtext_info.pool.number,
            sizeof(*dtext_info.pool.text)
        );
        for (int i = dtext_info.pool.idx; i < dtext_info.pool.number; ++i) {
            dtext_info.pool.text[i].raw = malloc(32);
            dtext_info.pool.text[i].size = 32;
        }
    }

    len = strlen(str) + 1;
    if (len >= dtext_info.pool.text[dtext_info.pool.idx].size) {
        dtext_info.pool.text[dtext_info.pool.idx].size = len;
        dtext_info.pool.text[dtext_info.pool.idx].raw = reallocarray(
            dtext_info.pool.text[dtext_info.pool.idx].raw,
            dtext_info.pool.text[dtext_info.pool.idx].size,
            sizeof(char)
        );
    }

    strcpy(dtext_info.pool.text[dtext_info.pool.idx].raw, str);
    return dtext_info.pool.text[dtext_info.pool.idx].raw;
}

//---
// Dstack API
//---

/* dfont_render() : draw caracter in surface */
void dtext_dstack(dsurface_t *surface, uintptr_t *arg)
{
    uint32_t code_point;
    uint8_t const * str;
    font_t *font;
    int glyph_idx;
    int glyph_width;
    int counter;
    int sx;
    int sy;

    /* check culling */
    if (((int)arg[3] < surface->y1 && (int)arg[1] > surface->y2)
        || ((int)arg[2] < surface->x1 && (int)arg[0] > surface->x2)) {
        return;
    }

    /* draw algo (update me) */
    sx = arg[0];
    sy = arg[1];
    counter = -1;
    font = dfont_get();
    str = (void*)arg[6];
    while (++counter < (int)arg[7])
    {
        code_point = dfont_utf8_next(&str);
        if (code_point == '\n') {
            arg[1] += font->glyph.height;
            arg[0] = sx;
            continue;
        }

        glyph_idx = dfont_glyph_index(font, code_point);
        glyph_width = font->glyph.mono.width;
        if (font->shape.prop == 1)
            glyph_width = font->glyph.prop[glyph_idx].width;

        dfont_char_render(surface, glyph_idx, arg);

        arg[0] += glyph_width + font->char_spacing;

    }
    arg[1] = sy;
    arg[0] = sx;
}

//---
// Public API
//---

/* dtext_opt(): Display a string of text */
did_t dtext_opt(
    int x, int y,
    int fg, int bg,
    int align,
    char const * const str, int size
) {
    size_t width;
    size_t height;
    void *real_str;

    /* get text geometry and handle obvious culling */
    if (dtext_geometry(str, &size, &width, &height) != 0)
        return (-1);
    if (x >= (int)dwidth()
            || y >= (int)dheight()
            || x + (int)width < 0
            || y + (int)height < 0) {
        return (-1);
    }

    /* handle position */
    if (align & DTEXT_CENTER) x = x - (width / 2);
    if (align & DTEXT_RIGHT)  x = x - (width);
    if (align & DTEXT_MIDDLE) y = y - (height / 2);
    if (align & DTEXT_BOTTOM) y = y - (height);

    /* register the string */
    real_str = dtext_info_register(str);

    /* request draw call */
    return dstack_add_action(
        DSTACK_CALL(
            &dtext_dstack,
            x, y, x + width, y + height,
            fg, bg,
            (uintptr_t)real_str, size,
        ),
        NULL,
        (dtext_info.pool.idx == 0) ? (void*)&__dtext_quit : NULL
    );
}

/* dtext() : display text information */
did_t dtext(int x, int y, int fg, char const * const text)
{
    return (dtext_opt(x, y, fg, C_NONE, DTEXT_LEFT | DTEXT_TOP, text, -1));
}
