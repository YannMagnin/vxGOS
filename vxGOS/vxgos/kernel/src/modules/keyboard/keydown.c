#include <vhex/keyboard.h>
#include <vhex/keyboard/keycache.h>
#include <stdarg.h>


/* keydown(): Current key state */
int keydown(vkey_t key)
{
    return keycache_keydown(key);
}

/* keydown_all(): Check a set of keys for simultaneous input */
int keydown_all(vkey_t key1, ...)
{
    int key;
    va_list ap;

    va_start(ap, key1);

    key = key1;
    do {
        if (keycache_keydown(key) == 0)
            return 0;
        key = va_arg(ap, vkey_t);
    } while (key != VKEY_NONE);

    va_end(ap);
    return 1;
}

/* keydown_any(): Check a set of keys for any input */
int keydown_any(vkey_t key1, ...)
{
    int key;
    va_list ap;

    va_start(ap, key1);

    key = key1;
    do {
        if (keycache_keydown(key) != 0)
            return 1;
        key = va_arg(ap, vkey_t);
    } while (key != VKEY_NONE);

    va_end(ap);
    return 0;
}
