#if 0
//FIXME : driver-specific
//FIXME : redirect custom mode for getkey_opt
#include <vhex/keyboard.h>
#include <vhex/keyboard/keycache.h>

/* getkey(): Wait for a key press */
key_event_t getkey(void)
{
    return getkey_opt(GETKEY_DEFAULT, NULL);
}

/* getkey_opt(): Enhanced getkey() */
key_event_t getkey_opt(int options, volatile int *timeout)
{
    key_event_t e;

    while (1)
    {
        keycache_event_pop(&e);
        if(e.type == KEYEV_NONE) {
            keycache_event_wait(timeout);
            continue;
        }

        (void)options;
        //TODO: handle modifier
        //TODO: handle repeat
        //TODO: handle feature
        //TODO: handle timeout

        return e;
    }
}
#endif
