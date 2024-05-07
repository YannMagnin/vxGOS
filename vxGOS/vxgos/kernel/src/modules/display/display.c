//---
// modules:display:module   - expose the display module
//---

#include "vhex/modules/module.h"

//---
// Internals
//---

/* external symbols */
extern void __dtext_constructor(void);
extern void __dtext_destructor(void);
extern void __dstack_constructor(void);
extern void __dstack_destructor(void);

/* __display_init() : initialize the display */
static void __display_init(void)
{
    __dstack_constructor();
    __dtext_constructor();
}

/* __display_quit() : uninit the display */
static void __display_quit(void)
{
    __dtext_destructor();
    __dstack_destructor();
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
