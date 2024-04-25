#include "bootloader/console.h"
#include "fxcp400/keycode.h"

//---
// Internlas
//---

/* keysc_fetch() : scan the KEYSC hardware module */
static int keysc_fetch(void)
{
    uint16_t volatile *SH7305_KEYSC = (void*)0xa44b0000;
    int column;
    int key;
    int row;
    int registered;
    uint16_t udata;

    row = 6;
    key = 0x5f;
    registered = 0x0000;
    while (--row >= 0)
    {
        column = 16;
        udata = SH7305_KEYSC[row];
        if (registered != 0x0000)
            continue;
        while (--column >= 0)
        {
            if ((udata & (0x8000 >> column)) != 0) {
                registered = KEYCODE_GEN(row, column);
                break;
            }
            key -= 1;
        }
    }
    return registered;
}

//---
// Public
//---

/* console_key_get() : small one-shot key waiter */
int console_key_get(void)
{
    static uint16_t prev_key = 0xffff;

    if (prev_key != 0xffff) {
        while (1) {
            if (prev_key != keysc_fetch())
                break;
        }
    }
    while (1) {
        prev_key = keysc_fetch();
        if (prev_key != 0x0000)
            break;
    }
    return prev_key;
}
