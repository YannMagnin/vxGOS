#include <vhex/keyboard.h>
#include <vhex/module.h>

/* module init */

/* __keyboard_init() : initialize the keyboard */
static void __keyboard_init(void)
{
    extern void keycache_init(void);
    keycache_init();
}

/* __keyboard_quit() : uninit the keyboard */
static void __keyboard_quit(void)
{
    /* Nothing to do here ? */
}

/* declare the keyboard module */

struct vhex_module mod_keyboard = {
    .name = "keyboard",
    .init = &__keyboard_init,
    .quit = &__keyboard_quit,
};
VHEX_DECLARE_MODULE(05, mod_keyboard);
