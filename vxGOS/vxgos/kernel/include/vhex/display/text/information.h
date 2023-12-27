#ifndef __VHEX_DISPLAY_TEXT_INFORMATION__
# define __VHEX_DISPLAY_TEXT_INFORMATION__

#include <vhex/defs/types.h>

/* dtext_geometry() : get the rendered text geometry with the current font */
extern int dtext_geometry(char const * const str, int *n, size_t *w, size_t *h);

#endif /* __VHEX_DISPLAY_TEXT_INFORMATION__ */
