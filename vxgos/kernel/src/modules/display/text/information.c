#include <vhex/display/types.h>
#include <vhex/display/font/information.h>

/* dtext_geometry() : get the rendered text geometry with the current font */
int dtext_geometry(char const * const str_char, int *s, size_t *w, size_t *h)
{
    return dfont_text_geometry(NULL, str_char, s, w, h);
}
