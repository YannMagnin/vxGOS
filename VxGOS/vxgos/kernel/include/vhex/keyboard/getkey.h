#ifndef __VHEX_KEYBOARD_GETKEY__
# define __VHEX_KEYBOARD_GETKEY__

#include <vhex/keyboard/types.h>

//---
//  High-level functions
//---

/* getkey(): Wait for a key press

   This function mimics the behavior of the fxlib GetKey(). It returns a
   key_event_t object where [mod=1], and where [shift] and [alpha] indicate
   whether SHIFT or ALPHA was pressed before the key was hit. [event] is
   VKEYEV_DOWN when a new key is pressed and VKEYEV_HOLD in case of repeats.

   Similarities with GetKey() include:
   - Wait for a key to be pressed *after* the call (held keys don't count)
   - Supports SHIFT and ALPHA modifiers
   - Repeats arrows keys
   - Allows return to main menu if the MENU key is pressed
   - Controls backlight on models that have a back-lit screen

   getkey() is equivalent to getkey_opt(GETKEY_DEFAULT, NULL). */
extern vkey_event_t getkey(void);

/* The following are the option bits for getkey_opt(). */
enum {
    /* Enable modifiers keys */
    GETKEY_MOD_SHIFT   = 0x01,
    GETKEY_MOD_ALPHA   = 0x02,
    /* SHIFT + OPTN toggles backlight (requires GETKEY_MOD_SHIFT) */
    GETKEY_BACKLIGHT   = 0x04,
    /* MENU triggers a task switch and displays the main menu */
    GETKEY_MENU        = 0x08,
    /* Repeat arrow keys, or even all keys */
    GETKEY_REP_ARROWS  = 0x10,
    GETKEY_REP_ALL     = 0x20,
    /* Enable custom repeat profiles; see getkey_set_repeat_profile() */
    GETKEY_REP_PROFILE = 0x40,
    /* Enable application shortcuts; see getkey_set_feature_function() */
    GETKEY_FEATURES    = 0x80,

    /* No modifiers */
    GETKEY_NONE        = 0x00,
    /* Default settings of getkey() */
    GETKEY_DEFAULT     = 0xdf,
};

/* getkey_profile_t: Custom repeat profile function
   See getkey_set_repeat_profile() for details. */
typedef int (*getkey_profile_t)(int key, int duration, int count);

/* getkey_feature_t: Custom feature function
   See getkey_set_feature_function() for details. */
typedef bool (*getkey_feature_t)(vkey_event_t event);

/* getkey_opt(): Enhanced getkey()

   This function enhances getkey() with more general features. An
   or-combination of option flags (see above) must be supplied as first
   argument; GETKEY_NONE stands for no option. getkey_opt() returns the same
   kind of events as getkey().

   getkey_opt() supports a generic timeout function in the form of a volatile
   pointer [timeout]. If it's NULL, getkey_opt() waits indefinitely. Otherwise,
   it waits until *timeout becomes non-zero. It's up to you to change the
   value whenever you want to interrupt the call; using a timer with
   [timer_timeout] as callback is suitable.

   Event transforms in getkey_opt() (SHIFT, ALPHA and repetitions) are handled
   by changing the transform settings on the keyboard device. These settings
   are restored when getkey_opt() returns, so if they are originally disabled
   (which they are unless set manually) then the status of the SHIFT and ALPHA
   keys is lost between calls (this has an effect it getkey_opt() is
   interrupted by timeout). Therefore, in order to use modifiers across several
   calls to getkey_opt(), make sure to enable the transforms on the keyboard
   device.

   @options  An or-combination of values from the GETKEY_* enumeration
   @timeout  Optional pointer to a timeout value
   Returns a key event of type VKEYEV_DOWN or VKEYEV_HOLD with [mod=1]. */
extern vkey_event_t getkey_opt(int options, volatile int *timeout);

/* getkey_repeat(): Set repeat delays for getkey()

   This function updates the repeat delays of getkey() and getkey_opt(). The
   unit of the argument is in milliseconds, but the granularity of the delay is
   dependent on the keyboard scan frequency.

   In the default setting (128 Hz scans), the possible repeat delays are
   approximately 8 ms, 16 ms, 23 ms, 31 ms... the provided arguments will be
   rounded to the closest feasible delays to ensure that repetitions are
   perfectly regular, rather than approximating the requested frequency.

   The system default is (500 ms, 125 ms). With the 128 Hz setting, this
   default is reached exactly without approximation. vhex's default is (400 ms,
   40 ms) for more reactivity.

   @first  Delay between key press and first repeat (no more than one hour)
   @next   Delay between subsequent repeats (no more than one hour) */
extern void getkey_repeat(int first, int next);


#endif /* __VHEX_KEYBOARD_GETKEY__ */

