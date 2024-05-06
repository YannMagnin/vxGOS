#ifndef __VHEX_KEYBOARD_KEYCACHE__
# define __VHEX_KEYBOARD_KEYCACHE__

#include <vhex/keyboard/types.h>

/* keycache_init() : initialize the keycache */
extern void keycache_init(void);

/* keycache_keydown() : check the current key state */
extern int keycache_keydown(vkey_t key);

/* keycache_event_wait() : wait until the next event or the timeout != 0 */
extern int keycache_event_wait(vkey_event_t *event, volatile int *timeout);

/* keycache_event_pop() : pop the first key event in the cache */
extern int keycache_event_pop(vkey_event_t *event);

/* keycache_event_push() : push a key event in the cache */
extern int keycache_event_push(vkey_event_t *event);

/* keycache_init() : uninit the keycache */
extern void keycache_quit(void);


#endif /* __VHEX_KEYBOARD_KEYCACHE__ */
