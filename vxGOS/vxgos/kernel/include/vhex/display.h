#ifndef __VHEX_DISPLAY__
# define __VHEX_DISPLAY__

#include <vhex/defs/types.h>
#include <vhex/display/types.h>
#include <vhex/display/draw.h>
#include <vhex/display/color.h>
#include <vhex/display/text.h>
#include <vhex/display/image.h>
#include <vhex/display/stack.h>

//TODO: doc

//---
// User-level API
//---

/* dclear(): Fill the screen with a single color */
extern did_t dclear(int color);

/* dupdate(): Generate the display frame on the screen */
extern void dupdate(void);

/* dwidth(): get the screen width */
VINLINE size_t dwidth(void)
{
    return dstack_display_width();
}

/* dheight(): get the heigth of the screen */
VINLINE size_t dheight(void)
{
    return dstack_display_height();
}






//---
// Kernel-level API (move me ?)
//---

/* dclear_render() : internal pipeline drawing */
extern void dclear_render(dsurface_t *surface, uint32_t color);

#endif /*__VHEX_DISPLAY__*/
