#include <stdbool.h>

#include "bootloader/console.h"
#include "fxcp400/keycode.h"

//---
// Internlas
//---

/* keysc_fetch() : scan the KEYSC hardware module
 *
 * @note
 * - we MUST walk trough all KEYSC data entries to avoid hardware freeze
 * */
int keysc_fetch(void)
{
    uint16_t volatile *SH7305_KEYSC = (void*)0xa44b0000;
    uint16_t registered;
    uint16_t udata;

    registered = 0x0000;
    for (int row = 0; row < 6; ++row)
    {
        udata = SH7305_KEYSC[row];
        if (registered != 0x0000)
            continue;
        for (int column = 0 ; column < 16 ; ++column)
        {
            if ((udata & (0x0001 << column)) == 0)
                continue;
            registered = KEYCODE_GEN(row, column);
            break;
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
        while (true) {
            if (prev_key != keysc_fetch())
                break;
        }
    }
    while (true) {
        prev_key = keysc_fetch();
        if (prev_key != 0x0000)
            break;
    }
    return prev_key;
}
