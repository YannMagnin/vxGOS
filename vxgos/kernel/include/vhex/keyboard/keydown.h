#ifndef __VHEX_KEYBOARD_KEYDOWN__
# define __VHEX_KEYBOARD_KEYDOWN__

#include <vhex/keyboard/types.h>
#include <vhex/keyboard/keycode.h>

//---
//  Key state functions
//---

/* keydown(): Current key state
   This function returns zero if the specified key is currently up (according
   to the last events that have been processed) and non-zero if it is down. */
extern int keydown(vkey_t key);

/* keydown_all(): Check a set of keys for simultaneous input
   Returns non-zero if all provided keys are down. The list should end with
   VKEY_NONE as terminator. */
extern int keydown_all(vkey_t key1, ...);

/* keydown_any(): Check a set of keys for any input
   Returns nonzero if any one of the specified keys is currently pressed. The
   sequence should be terminated by VKEY_NONE. */
extern int keydown_any(vkey_t key1, ...);



#endif /* __VHEX_KEYBOARD_KEYDOWN__ */
