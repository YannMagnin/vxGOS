#ifndef __VHEX_KEYBOARD_TYPES__
# define __VHEX_KEYBOARD_TYPES__

#include <vhex/defs/types.h>
#include <vhex/defs/attributes.h>

#include <vhex/keyboard/keycode.h>

/* key_event_t: Low-level or high-level keyboard event

   This structure represents an event that occurs on the keyboard. It is first
   produced by the keyboard scanner with limited information, then possibly
   enriched by getkey(). Events are produced each time the keyboard emit
   interruption.

   getkey() returns enriched events with [mod=1], in whic ase [shift] and
   [alpha] indicate whether the key has been modified. Only key press events
   returned by getkey() have [mod=1]. Note that you can't have, e.g.
   [key=VKEY_SHIFT] and [mod=1] at the same time.

   The [time] attribute indicates when the event occurred. It is a snapshot of
   a time counter that increases at each keyboard scan and *wraps around every
   8 minutes* (at 128 Hz). I expect this attribute to be useful to analyze
   combo sequences in games. Make sure you are aware of the two nitpicks:
   * Don't keep the time values for too long because of the wrap-around effect.
   * 0xffff is "just before" 0x0000, not "long after". */
typedef struct
{
    uint time       :16;    /* Time of event, unique over short periods */

    uint rep_type   :3;     /* Repetition type */

    uint mod        :1;     /* Whether modifiers are used */
    uint shift      :1;     /* If mod=1, whether SHIFT was pressed */
    uint alpha      :1;     /* If mod=1, whether ALPHA was pressed */

    uint type       :2;     /* Type of key event */
    uint key        :8;     /* Hit key */

} VPACKED(4) vkey_event_t;

/* Keyboard event types, as in the [type] field of key_event_t */
enum
{
    VKEYEV_NONE         = 0, /* No event available (poll() only) */
    VKEYEV_DOWN         = 1, /* Key was pressed */
    VKEYEV_UP           = 2, /* Key was released */
    VKEYEV_HOLD         = 3, /* A key that was pressed has been held down */
};

/* keyboard repetition type */
enum
{
    VKEYEV_REP_NONE     = 0,
    VKEYEV_REP_FIRST    = 1,
    VKEYEV_REP_NEXT     = 2,
};


#endif /* __VHEX_KEYBOARD_TYPES__ */
