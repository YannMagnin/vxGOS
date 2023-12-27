#include "vhex/display/stack.h"
#include "vhex/module.h"

/* module init */

/* __display_init() : initialize the display */
static void __display_init(void)
{
    dstack_init();
}

/* __display_quit() : uninit the display */
static void __display_quit(void)
{
    dstack_quit();
}

/* declare the display module */

struct vhex_module mod_display = {
    .name = "display",
    .init = &__display_init,
    .quit = &__display_quit,
};
VHEX_DECLARE_MODULE(03, mod_display);
