#ifndef __VHEX_KEYBOARD_INTERFACE__
# define __VHEX_KEYBOARD_INTERFACE__

#include <vhex/keyboard/types.h>

/* keyboard_drv_interface - driver interface */
struct keyboard_drv_interface
{
    int (*keycache_init)(void);
    int (*keycache_keydown)(vkey_t key);
    int (*keycache_event_wait)(vkey_event_t *event, volatile int *timeout);
    int (*keycache_event_push)(vkey_event_t *event);
    int (*keycache_event_pop) (vkey_event_t *event);
    int (*keycache_quit)(void);
};

#endif /* __VHEX_KEYBOARD_INTERFACE__ */
