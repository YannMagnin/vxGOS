#ifndef __VHEX_KEYBOARD_KEYVENT__
# define __VHEX_KEYBOARD_KEYVENT__

#include <vhex/keyboard/types.h>

//---
//  Event-level functions
//---

/* keyvent_poll(): Poll the next keyboard event
   This function pop the next event from the event queue, chronologically.
   If no event is available, it will set a dummy event with type=VKEYEV_NONE
   and time set to the current driver time.

   This function always returns:
   <> positive value   if there is always pending event
   <> zero             if there is no more pending event
   <> negative value   if error */
extern int keyvent_poll(vkey_event_t *event);

/* waitevent(): Wait for the next keyboard event
   This function works as pollevent() but waits if no event is available. When
   timeout=NULL, it waits indefinitely. Otherwise, it waits until *timeout
   becomes non-zero (by using a timer for exemple). */
extern int keyvent_wait(vkey_event_t *event, volatile int *timeout);

/* clearevents(): Clear all events waiting in the queue */
extern void keyvent_clear(void);

#endif /* __VHEX_KEYBOARD_KEYVENT__ */
