#include "vhex/display/stack.h"
#include "vhex/module.h"

//---
// Internals
//---

/* external symbols */
extern void __dtext_constructor(void);
extern void __dtext_destructor(void);


/* __display_init() : initialize the display */
static void __display_init(void)
{
    dstack_init();
    __dtext_constructor();
}

/* __display_quit() : uninit the display */
static void __display_quit(void)
{
    dstack_quit();
    __dtext_destructor();
}

//---
// Public
//---

/* declare the display module */

struct vhex_module mod_display = {
    .name = "display",
    .init = &__display_init,
    .quit = &__display_quit,
};
VHEX_DECLARE_MODULE(03, mod_display);
